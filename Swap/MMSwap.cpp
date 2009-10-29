/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "MMSwap.hpp"
#include <File/LocalFile.hpp>
#include <Swap/MMBlock.hpp>

CMMSwap * CMMSwap::m_pSwap = NULL;

void CMMSwap :: InitializeSwap(void) {
    assert(! CMMSwap::m_pSwap);
    void * pMem = malloc(sizeof(CMMSwap));
    CMMSwap::m_pSwap = new(pMem) CMMSwap;
}

void CMMSwap :: UnInitializeSwap(void) {
    CMMSwap::m_pSwap->~CMMSwap();
    free(CMMSwap::m_pSwap);
    CMMSwap::m_pSwap = NULL;
}

#ifdef _WIN32
#define MM_FSIZE 32 * MBYTE
#endif

CMMSwap::CMMSwap(void) {
    m_Enabled = false;
    m_pvBlocks = NULL;
#ifdef _UNIX
    m_Descriptor = -1;
#endif
#ifdef _WIN32
    m_DescCount = 0;
#endif
    // _L_DEBUG(1, cout << "CMMSwap :: CMMSwap" << endl);  
}

void CMMSwap::SetEnabled(bool bValue, char * pszFilename) {
    // enabling
    if (bValue && !m_Enabled) {    
        // on unix, initialize a growing mmaped file, on nt, late temp file creation
    
#ifdef _WIN32
        if (pszFilename) {
            cerr << "MMSwap: file name specification for the swap file is ignored under Windows NT." << endl;
        }
#endif
    
#ifdef _UNIX
        if (pszFilename) {
            int nLen = strlen(pszFilename);
            if (nLen >= MAXPATHLEN) {
                cerr << "MMSwap :: Temporary filename specification too long (" << pszFilename << ")" << endl;
                CHandler :: Terminate(-1);
            }
            memcpy(m_szFilename, pszFilename, (nLen + 1) * sizeof(char));
            m_Descriptor = base_open((const char *) m_szFilename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);          
        } else {
            m_Descriptor = CLocalFile::GetTmpDescriptorSz(m_szFilename);  
        }
      
        // _L_DEBUG(1, cout << "CMMSwap :: Opened " << m_szFilename << " [" << m_Descriptor << "]" << endl);
        if (m_Descriptor == -1) {
            cerr << "MMSwap :: Unable to open a temporary file (" << m_szFilename << ")" << endl;
            CHandler :: Terminate(-1);
        }
#endif  
        
        m_TailOffset = 0;
        m_LastMultiple = 0;
        
#ifdef _UNIX
        m_PageSize = getpagesize();
#endif
        
#ifdef _WIN32
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        m_PageSize = SystemInfo.dwAllocationGranularity;
#endif 
        
        memset(m_CacheBlocks, 0, MM_MAX_CACHE * sizeof(CMMBlock *));
    }
    
    if (!bValue && m_Enabled) {
#ifdef _WIN32
        for (register int i=0; i < m_DescCount; i++) {
            CloseHandle(m_Mappings[i]);
            CloseHandle(m_Descriptors[i]);
            DeleteFile(m_szFilenames[i]);    
        }
#endif
#ifdef _UNIX
        if (m_Descriptor >= 0) {
            base_close(m_Descriptor);
            base_unlink(m_szFilename);
        }
#endif
    }
    
    m_Enabled = bValue;
}

CMMSwap::~CMMSwap(void) {  
    SetEnabled(false);
    FreeBlocks();    
}

void CMMSwap::FreeBlocks(void) {
    CMMBlock * pBlock = m_pvBlocks;
    while (pBlock) {
        pBlock = m_pvBlocks->m_pNext;
        m_pvBlocks->~CMMBlock();        
        free(m_pvBlocks);
        m_pvBlocks = pBlock;
    }
}

void * CMMSwap::MMAllocate(size_t DesiredBytes) { 

	//assert(DesiredBytes < (1024000));

    if (!m_Enabled)
        return malloc(DesiredBytes);

    // make Bytes a multiple of 32 nomatter what in order not to have too much fragmentation
    size_t Bytes = 8;
    while (Bytes < DesiredBytes)
        Bytes <<= 1;
    
    m_IrlMutex.StartReading();
    // find a block of the right size with at least one free element
    // _L_DEBUG(1, cout << "CMMSwap :: looking up a block of " << Bytes << endl);
    bool bNewBlock = true;  
    int nAcquiredBit = -1;

    // look inside the pool of cached blocks for each multiple of 32 bytes
    short int nCachedIndex = (Bytes / 32) % MM_MAX_CACHE;

    CMMBlock * CurrentBlock = m_CacheBlocks[nCachedIndex];

    if ((CurrentBlock) &&
        (CurrentBlock->m_nElSize == Bytes) &&
        ((nAcquiredBit = CurrentBlock->AcquireFree()) != -1)) {
            bNewBlock = false;
    } else if (Bytes <= (unsigned long) m_LastMultiple) {        
        CurrentBlock = m_pvBlocks; // (CMMBlock *) m_vBlocks.GetLast(Iterator);
        while (CurrentBlock) {
            if (CurrentBlock->m_nElSize == Bytes) {
                nAcquiredBit = CurrentBlock->AcquireFree();
                if (nAcquiredBit != -1) {
                    bNewBlock = false;
                    break;
                }
            }
            CurrentBlock = CurrentBlock->m_pNext; // (CMMBlock *) m_vBlocks.GetPrev(Iterator);
        }
    } else {
       m_LastMultiple = Bytes;       
    }
    m_IrlMutex.StopReading();    

    
    // find a multiple of page size - this is the block that we will in fact allocate
    size_t MappedRegionSize = m_PageSize;
    while ((Bytes > MappedRegionSize) || (MappedRegionSize < 64 * KBYTE))
        MappedRegionSize <<= 1;
    
    //  win32: do we have enough space in the current temporary file size?
#ifdef _WIN32
    
    // cout << "bNewBlock: " << bNewBlock << endl;
    // cout << "TailOffset: " << m_TailOffset << endl;
    // cout << "MappedSize: " << MappedRegionSize << endl;
    // cout << "FSize: " << MM_FSIZE << endl;
    
    if (!m_DescCount || (bNewBlock && (m_TailOffset + MappedRegionSize > MM_FSIZE))) {
        m_IrlMutex.StartWriting();
        if (!m_DescCount || (bNewBlock && (m_TailOffset + MappedRegionSize > MM_FSIZE))) {
            if (m_DescCount++ == MM_MAX_DESC) {
                cerr << "MMSwap :: maximum amount of temporary swap files reached." << endl;
                CHandler :: Terminate(-1);
            }
            m_Descriptors[m_DescCount - 1] = CLocalFile::GetTmpHandleSz(m_szFilenames[m_DescCount - 1]);
            if (m_Descriptors[m_DescCount - 1] == (void *) -1) {
                cout << "Swap: Error in GetTmpHandleSz (" << m_szFilenames[m_DescCount - 1] << ")" << endl;
                CObject :: ShowLastError();
                CHandler :: Terminate(-1);
            }
            size_t MappedFileSize = ((MappedRegionSize>MM_FSIZE)?MappedRegionSize:MM_FSIZE);
            m_Mappings[m_DescCount - 1] = CreateFileMapping(
                m_Descriptors[m_DescCount - 1], 
                NULL, 
                PAGE_READWRITE, 
                0, 
                MappedFileSize, 
                NULL);
            if (!m_Mappings[m_DescCount - 1]) {
                cerr << "Swap: Error in CreateFileMapping (" << m_szFilenames[m_DescCount - 1] << "/" << MappedFileSize << ")" << endl;
                CObject :: ShowLastError();
                CHandler :: Terminate(-1);
            }    
            m_TailOffset = 0;
        }
        m_IrlMutex.StopWriting();
    }
#endif
    
    if (bNewBlock) {    
        // add a new clean one
        // _L_DEBUG(1, cout << "CMMSwap :: adding a block of " << Bytes << endl);
        
        void * pMem = malloc(sizeof(CMMBlock));
        CMMBlock * pNewBlock = new(pMem) CMMBlock(Bytes);        

        m_IrlMutex.StartWriting();
        // m_vBlocks += NewBlock;

        pNewBlock->m_pNext = m_pvBlocks;
        m_pvBlocks = pNewBlock;
        
        CurrentBlock = pNewBlock;
        
        // initialize the new block
        m_TailOffset = CurrentBlock->Initialize(
            m_TailOffset, 
#ifdef _WIN32
            m_Descriptors[m_DescCount - 1], 
            m_Mappings[m_DescCount - 1],
#endif
#ifdef _UNIX
            m_Descriptor,
#endif
            MappedRegionSize);
        
        nAcquiredBit = CurrentBlock->AcquireFree();
        // assert(nAcquiredBit != -1);

        m_CacheBlocks[nCachedIndex] = CurrentBlock;

        m_IrlMutex.StopWriting();
        // printf("CMMSwap :: new file size: %ld bytes.\n", m_TailOffset);
    } else {
        m_IrlMutex.StartWriting();
        m_CacheBlocks[nCachedIndex] = CurrentBlock;
        m_IrlMutex.StopWriting();
    }
    
    // _L_DEBUG(1, cout << "CMMSwap :: new file size: " << m_TailOffset << " bytes." << endl);
    
    // add a new element to the block
    void * pMem = CurrentBlock->Allocate(nAcquiredBit);  
    
    // assert(pMem);
    
    // _L_DEBUG(1, cout << "CMMSwap :: allocated pointer: " << (long) pMem << endl);
    
    // printf("[+%d]", CurrentBlock->m_nElSize);
    return pMem;
}

void CMMSwap::MMFree(void * Pointer) {

    if (!m_Enabled) {
        free(Pointer);
        return;
    }
    
    m_IrlMutex.StartReading();
    // find a block of the right size 
    // _L_DEBUG(1, cout << "CMMSwap :: looking up a block to free " << (long) Pointer << endl);
    bool bFree = false;    
    for (int nCachedIndex = 0; nCachedIndex < MM_MAX_CACHE; nCachedIndex++) {
        if ((m_CacheBlocks[nCachedIndex]) && (m_CacheBlocks[nCachedIndex]->Free((char *) Pointer))) {
            bFree = true;
            break;
        }
    }
    if (!bFree) {
        CMMBlock * CurrentBlock = m_pvBlocks; // (CMMBlock *) m_vBlocks.GetLast(Iterator);
        while (CurrentBlock) {
            if (CurrentBlock->Free((char *) Pointer)) {
                // _L_DEBUG(1, cout << "CMMSwap :: successfuly freed " << (long) Pointer << endl);      
                break;
            }
            CurrentBlock = CurrentBlock->m_pNext; // (CMMBlock *) m_vBlocks.GetPrev(Iterator);
        }
    }
    m_IrlMutex.StopReading();  
    // assert(CurrentBlock);
    // printf("[-%d]", CurrentBlock->m_nElSize);
    // _L_DEBUG(1, cout << "CMMSwap :: MMFree returned for " << (long) Pointer << endl);
}

long CMMSwap::GetSize(void) const { 
#ifdef _WIN32
    long lResult = 0;
    for (register int i=0; i < m_DescCount; i++) {
        lResult += CLocalFile::GetFileSize(m_szFilenames[i]);
    }
    return lResult;
#endif
#ifdef _UNIX
    return m_TailOffset; 
#endif
}

