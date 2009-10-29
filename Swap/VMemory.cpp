/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "VMemory.hpp"

CVMemory CVMemory::m_VMemory;

CVMemory::CVMemory(void) {
    m_LastMultiple = 0;
#ifdef _UNIX
    m_PageSize = getpagesize();
#endif    
#ifdef _WIN32
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    m_PageSize = SystemInfo.dwAllocationGranularity;
#endif 
       
    memset(m_CacheBlocks, 0, V_MAX_CACHE * sizeof(CABlock<char> *));
}

CVMemory::~CVMemory(void) {  
  
}

void * CVMemory::VAllocate(size_t DesiredBytes) { 
    // make Bytes a multiple of 32 nomatter what in order not to have too much fragmentation
    size_t Bytes = 8;
    while (Bytes < DesiredBytes)
        Bytes <<= 1;
    
    m_IrlMutex.StartReading();
    // find a block of the right size with at least one free element
    // _L_DEBUG(1, cout << "CVMemory :: looking up a block of " << Bytes << endl);
    bool bNewBlock = true;  
    int nAcquiredBit = -1;

    // look inside the pool of cached blocks for each multiple of 32 bytes
    short int nCachedIndex = (Bytes / 32) % V_MAX_CACHE;

    CIterator Iterator;
    CABlock<char> * CurrentBlock = m_CacheBlocks[nCachedIndex];

    if ((CurrentBlock) &&
        (((CTBlock<char> *) CurrentBlock)->GetElSize() == Bytes) &&
        ((nAcquiredBit = ((CTBlock<char> *) CurrentBlock)->AcquireFree()) != -1)) {
            bNewBlock = false;
    } else if (Bytes <= (unsigned long) m_LastMultiple) {        
        CurrentBlock = (CABlock<char> *) m_vBlocks.GetLast(Iterator);
        while (CurrentBlock) {
            if (((CTBlock<char> *) CurrentBlock)->GetElSize() == Bytes) {
                nAcquiredBit = ((CTBlock<char> *) CurrentBlock)->AcquireFree();
                if (nAcquiredBit != -1) {
                    bNewBlock = false;
                    break;
                }
            }
            CurrentBlock = (CABlock<char> *) m_vBlocks.GetPrev(Iterator);
        }
    } else {
       m_LastMultiple = Bytes;       
    }
    m_IrlMutex.StopReading();    

    
    // find a multiple of page size - this is the block that we will in fact allocate
    size_t MappedRegionSize = m_PageSize;
    while ((Bytes > MappedRegionSize) || (MappedRegionSize < 64 * KBYTE))
        MappedRegionSize <<= 1;
        
    if (bNewBlock) {    
        // add a new clean one
        // _L_DEBUG(1, cout << "CVMemory :: adding a block of " << Bytes << endl);
        CABlock<char> NewBlock(Bytes);
        m_IrlMutex.StartWriting();
        m_vBlocks += NewBlock;    
        CurrentBlock = (CABlock<char> *) m_vBlocks.GetLast(Iterator);
        
        ((CABlock<char> *) CurrentBlock)->Initialize(MappedRegionSize);
        
        nAcquiredBit = ((CTBlock<char> *) CurrentBlock)->AcquireFree();
        // assert(nAcquiredBit != -1);
        m_IrlMutex.StopWriting();
        // printf("CVMemory :: new file size: %ld bytes.\n", m_TailOffset);
    }

    m_IrlMutex.StartWriting();
    m_CacheBlocks[nCachedIndex] = CurrentBlock;
    m_IrlMutex.StopWriting();
    
    // add a new element to the block
    void * pMem = (void *) ((CTBlock<char> *) CurrentBlock)->Allocate(nAcquiredBit);  
    
    // assert(pMem);
    
    // _L_DEBUG(1, cout << "CVMemory :: allocated pointer: " << (long) pMem << endl);
    
    // printf("[+%d]", CurrentBlock->m_nElSize);
    return pMem;
}

void CVMemory::VFree(void * Pointer) {

    m_IrlMutex.StartReading();
    // find a block of the right size 
    // _L_DEBUG(1, cout << "CVMemory :: looking up a block to free " << (long) Pointer << endl);
    bool bFree = false;    
    for (int nCachedIndex = 0; nCachedIndex < V_MAX_CACHE; nCachedIndex++) {
        if ((m_CacheBlocks[nCachedIndex]) && (((CTBlock<char> *) m_CacheBlocks[nCachedIndex])->Free((char *) Pointer))) {
            bFree = true;
            break;
        }
    }
    if (!bFree) {
        CIterator Iterator;
        CABlock<char> * CurrentBlock = (CABlock<char> *) m_vBlocks.GetLast(Iterator);
        while (CurrentBlock) {
            if (((CTBlock<char> *) CurrentBlock)->Free((char *) Pointer)) {
                // _L_DEBUG(1, cout << "CVMemory :: successfuly freed " << (long) Pointer << endl);      
                break;
            }
            CurrentBlock = (CABlock<char> *) m_vBlocks.GetPrev(Iterator);
        }
    }
    m_IrlMutex.StopReading();  
    // assert(CurrentBlock);
    // printf("[-%d]", CurrentBlock->m_nElSize);
    // _L_DEBUG(1, cout << "CVMemory :: VFree returned for " << (long) Pointer << endl);
}

long CVMemory::GetSize(void) const { 
  CIterator Iterator;
  long lResult = 0;
  
  m_IrlMutex.StartReading();
  CABlock<char> * CurrentBlock = (CABlock<char> *) m_vBlocks.GetLast(Iterator);
  while (CurrentBlock) {
    lResult += ((CTBlock<char> *) CurrentBlock)->GetSize();
    CurrentBlock = (CABlock<char> *) m_vBlocks.GetPrev(Iterator);
  }
  m_IrlMutex.StopReading();
  
  return lResult;
}

