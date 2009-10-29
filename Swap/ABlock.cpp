/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
    
*/

#include <baseclasses.hpp>
#include "ABlock.hpp"

template <class T>
CABlock<T>::CABlock(size_t nElSize) : CTBlock<T>(nElSize) {
  
}

template <class T>
void CABlock<T>::Initialize(size_t ElCount) {    
    m_Mutex.Lock();
    VInitialize(ElCount * sizeof(T));	
    m_pMappedRegion = new T[ElCount];
    // cout << "Mapped region is " << (long) m_pMappedRegion << " => " << (long) m_pMappedRegion + ElCount * sizeof(T) << endl;
    // cout << "            with " << ElCount << " free elements." << endl;
    m_Mutex.UnLock();
}

template <class T>
CABlock<T>::~CABlock(void) {
    if (m_pMappedRegion)
      delete[] (m_pMappedRegion);
}
