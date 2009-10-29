/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
    
*/

#include <baseclasses.hpp>
#include "MMBlock.hpp"

CMMBlock::CMMBlock(size_t nElSize) : CVBlock(nElSize) {
    m_pNext = NULL;
    m_AllocatedSet.SetbUseMalloc(true);
}

off_t CMMBlock::Initialize(off_t Offset, 
#ifdef _WIN32
                                  HANDLE Descriptor,
                                  HANDLE Mapping,
#endif
#ifdef _UNIX
                                  int Descriptor, 
#endif
                                  size_t MappedRegionSize) {
    
    m_Mutex.Lock();
    VInitialize(MappedRegionSize);

    m_Offset = Offset;
    off_t NewEndOffset = Offset + m_pMappedRegionSize;
    
    // _L_DEBUG(1, cout << "CMMSwap :: initializing new block of " << m_nFree << " elts, " << m_nElSize << " bytes each, between " << Offset << " and " << NewEndOffset << endl);
    
#ifdef _UNIX
    base_seek(Descriptor, Offset + m_pMappedRegionSize - 1, SEEK_SET);
    // write a byte to extend the file size
    base_write(Descriptor, (const char *) "\0", 1);
#endif
    
    // map the entire region
#ifdef _UNIX
    m_pMappedRegion = (char *) mmap(0, m_pMappedRegionSize, PROT_READ | PROT_WRITE, MAP_SHARED, Descriptor, m_Offset);
    if (m_pMappedRegion == (void *) -1) {
        cerr << "CMMSwap :: mmap failed." << endl;
        perror("CMMSwap");
        CHandler :: Terminate(-1);
    }
#endif
#ifdef _WIN32
    m_pMappedRegion = (char *) MapViewOfFile(Mapping, FILE_MAP_ALL_ACCESS, 0, m_Offset, m_pMappedRegionSize);
    if (!m_pMappedRegion) {
        cout << "Swap: Error in MapViewOfFile (" << m_Offset << ")" << endl;
        CObject::ShowLastError();
        CHandler :: Terminate(-1);
    }
#endif
    // _L_DEBUG(1, cout << "CMMSwap :: mmap returned " << (long) m_pMappedRegion << endl);
    // memset(m_pMappedRegion, 0, m_pMappedRegionSize);
    // _L_DEBUG(1, cout << "CMMSwap :: cleared block of " << m_pMappedRegionSize << " bytes." << endl);
    
    m_Mutex.UnLock();
    
    return NewEndOffset;
}

CMMBlock::~CMMBlock(void) {
#ifdef _UNIX
    munmap((caddr_t) m_pMappedRegion, m_pMappedRegionSize);
#endif
#ifdef _WIN32
    UnmapViewOfFile(m_pMappedRegion);
#endif    
}
