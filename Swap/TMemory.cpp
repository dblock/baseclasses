/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "TMemory.hpp"

template <class T>
T * CTMemory<T>::Allocate(void) {     

    m_IrlMutex.StartReading();
    // find a block of the right size with at least one free element
    // _L_DEBUG(1, cout << "CTMemory :: looking up a block of " << sizeof(T) << endl);
    bool bNewBlock = true;  
    int nAcquiredBit = -1;
    CIterator Iterator;
    CABlock<T>  * CurrentBlock = (CABlock<T>  *) m_vBlocks.GetLast(Iterator);
    while (CurrentBlock) {
        nAcquiredBit = CurrentBlock->AcquireFree();
        if (nAcquiredBit != -1) {
            bNewBlock = false;
            break;
        }
		CurrentBlock = (CABlock<T>  *) m_vBlocks.GetPrev(Iterator);
    }
    m_IrlMutex.StopReading();    
	
    if (bNewBlock) {    
        // add a new clean one
        // _L_DEBUG(1, cout << "CTMemory :: adding a block of " << sizeof(T) << endl);
        CABlock<T>  NewBlock(sizeof(T));
        m_IrlMutex.StartWriting();
        m_vBlocks += NewBlock;
        CurrentBlock = (CABlock<T>  *) m_vBlocks.GetLast(Iterator);
		
        CurrentBlock->Initialize(m_PageSize);
		
        nAcquiredBit = CurrentBlock->AcquireFree();
        // assert(nAcquiredBit != -1);
        m_IrlMutex.StopWriting();
        // printf("CTMemory :: new file size: %ld bytes.\n", m_TailOffset);
    }
	
    // add a new element to the block
    T * pMem = CurrentBlock->Allocate(nAcquiredBit);    
    // assert(pMem);
    
    // _L_DEBUG(1, cout << "CTMemory :: allocated pointer: " << (long) pMem << endl);
    
    // printf("[+%d]", CurrentBlock->m_nElSize);
    return pMem;
}

template <class T>
void CTMemory<T>::Free(T * Pointer) {
    m_IrlMutex.StartReading();
    CIterator Iterator;
    CABlock<T>   * CurrentBlock = (CABlock<T> *) m_vBlocks.GetLast(Iterator);
    while (CurrentBlock) {
        if (CurrentBlock->Free(Pointer)) {
            // _L_DEBUG(1, cout << "CTMemory :: successfuly freed " << (long) Pointer << endl);      
            break;
        }
        CurrentBlock = (CABlock<T>   *) m_vBlocks.GetPrev(Iterator);
    }
    m_IrlMutex.StopReading();  
    // assert(CurrentBlock);
    // printf("[-%d]", CurrentBlock->m_nElSize);
    // _L_DEBUG(1, cout << "CTMemory :: VFree returned for " << (long) Pointer << endl);
}

template <class T>
long CTMemory<T>::GetSize(void) const { 
  CIterator Iterator;
  long lResult = 0;
  
  m_IrlMutex.StartReading();
  CABlock<T>  * CurrentBlock = (CABlock<T>   *) m_vBlocks.GetLast(Iterator);
  while (CurrentBlock) {
    lResult += CurrentBlock->GetSize();
    CurrentBlock = (CABlock<T>  *) m_vBlocks.GetPrev(Iterator);
  }
  m_IrlMutex.StopReading();  
  return lResult;
}

template <class T>
CTMemory<T>::CTMemory(void) {    
#ifdef _UNIX
    m_PageSize = getpagesize();
#endif    
#ifdef _WIN32
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    m_PageSize = SystemInfo.dwAllocationGranularity;
#endif
}

template <class T>
CTMemory<T>::~CTMemory(void) {  
  
}
