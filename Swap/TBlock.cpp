/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
    
*/

#include <baseclasses.hpp>
#include "TBlock.hpp"

template <class T>
CTBlock<T>::CTBlock(size_t nElSize) {

  m_nFree = 0;
  m_nTotal = 0;
  m_nElSize = nElSize;
  m_pMappedRegion = NULL;
  m_pMappedRegionSize = 0;

}

template <class T>
void CTBlock<T>::VInitialize(size_t MappedRegionSize) {
    
    m_pMappedRegionSize = MappedRegionSize;    
    // page size is the size on disc
    // PageSize / m_nElSize is how many elements we can put in that bloc
    
    m_nFree = m_pMappedRegionSize / m_nElSize;
    m_nTotal = m_nFree;
    // assert(m_nFree);

    // cout << "Block of " << m_pMappedRegionSize << ", each element of " << m_nElSize << ", " << m_nTotal << " free elements." << endl;
    
    m_AllocatedSet.Grow(m_nFree);    
}

template <class T>
CTBlock<T>::~CTBlock(void) {

}

template <class T>
void CTBlock<T>::operator=(const CTBlock<T>& Block) {
  
  m_nFree = Block.m_nFree;
  m_nTotal = Block.m_nTotal;
  m_nElSize = Block.m_nElSize;    
  m_AllocatedSet = Block.m_AllocatedSet;

}


