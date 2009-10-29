/*

  Simple Virtual Memory Implementation
  ==================================================
  part of the BASE C++ Classes
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TBLOCK_HPP
#define BASE_TBLOCK_HPP

#include <platform/include.hpp>

#include <BitSet/BitSet.hpp>
#include <Object/Object.hpp>
#include <Mutex/Mutex.hpp>
#include <Mutex/RWMutex.hpp>
#include <List/List.hpp>

template <class T>
class CTBlock {
protected:
  CMutex         m_Mutex;
  T *            m_pMappedRegion;
  size_t         m_pMappedRegionSize;
  long           m_nFree;                                      // hint on free blocks
  size_t         m_nElSize;                                    // size of each element
  long           m_nTotal;
  CBitSet        m_AllocatedSet;
public:
  inline size_t& GetElSize(void) { return m_nElSize; }
  inline long& GetTotal(void) { return m_nTotal; }
  inline long& GetFree(void) { return m_nFree; }
  inline T * GetMappedRegion(void) { return m_pMappedRegion; }
  CTBlock(size_t nElSize = 0);
  void VInitialize(size_t MappedRegionSize);
  inline T * Allocate(int AcquiredBit);
  inline int AcquireFree(void);
  inline bool Free(T * Pointer);  
  virtual ~CTBlock(void);
  void operator=(const CTBlock<T>& Block);
  inline size_t GetSize(void) const { return m_pMappedRegionSize; }
  inline bool operator<(const CTBlock<T>& Block) const { return (m_pMappedRegion < Block.m_pMappedRegion)?true:false; }
  inline bool operator>(const CTBlock<T>& Block) const { return (m_pMappedRegion > Block.m_pMappedRegion)?true:false; }
  inline bool operator<=(const CTBlock<T>& Block) const { return (m_pMappedRegion <= Block.m_pMappedRegion)?true:false; }
  inline bool operator>=(const CTBlock<T>& Block) const { return (m_pMappedRegion >= Block.m_pMappedRegion)?true:false; }
  inline bool operator!=(const CTBlock<T>& Block) const { return (m_pMappedRegion != Block.m_pMappedRegion)?true:false; }
  inline bool operator==(const CTBlock<T>& Block) const { return (m_pMappedRegion == Block.m_pMappedRegion)?true:false; }
  inline ostream& operator<<(ostream& Stream) const { return Stream; }
  inline const T * GetFirst(CIterator&) const;
  inline const T * GetNext(CIterator&) const;
  inline const T * GetLast(CIterator&) const;
  inline const T * GetPrev(CIterator&) const;  
};

template <class T>
inline T * CTBlock<T>::Allocate(int AcquiredBit) {
        
    // assert(m_AllocatedSet.CountBits(true) == (m_nTotal - m_nFree));    
    // assert(AcquiredBit < m_nTotal);
  
    T * Ptr = (T *) ((char *) m_pMappedRegion + m_nElSize * AcquiredBit);

    // cout << "Mapped region is " << (long) m_pMappedRegion << " => " << (long) m_pMappedRegion + m_nTotal * sizeof(T) << endl;
    // cout << "       returning " << (long) Ptr << " (" << Ptr - m_pMappedRegion << ") for " << AcquiredBit << endl;
    // cout << "       remaining " << (long) m_pMappedRegion + (m_nTotal * sizeof(T)) - (long) Ptr << endl;

    return Ptr;
}

template <class T>
inline int CTBlock<T>::AcquireFree(void) {
    int nResult = -1;
    if (m_nFree) {
        m_Mutex.Lock();
        if (m_nFree) {
            
            m_nFree--;            
            
            nResult = m_AllocatedSet.GetFirstUnset(true);
            
        }
        m_Mutex.UnLock();
    }
    return nResult;
}


template <class T>
inline ostream& operator<<(ostream& Stream, const CTBlock<T>& Block) {
    return Block.operator<<(Stream);
}

template <class T>
inline bool CTBlock<T>::Free(T * Pointer) { 
     // pointer inside the region - can't divide directly, might fall into the block
     bool bResult = false;
     
     if ((Pointer >= m_pMappedRegion) &&
         (Pointer < m_pMappedRegion + (m_nElSize * (m_nTotal)))) {
         
         int PtrPosition = (int) (Pointer - m_pMappedRegion) / m_nElSize;    
 
         // assert(PtrPosition < (m_nFree + m_nUsed));           
         // assert(m_AllocatedSet.GetBit(PtrPosition) == 1);
     
         m_Mutex.Lock();
         m_AllocatedSet.UnSetBit(PtrPosition);        
 
         // assert(m_AllocatedSet.GetBit(PtrPosition) == 0);
         
         m_nFree++;
         m_Mutex.UnLock();        
                 
         // assert(m_AllocatedSet.CountBits(true) == m_nUsed);
         
         bResult = true;    
     }
     
     return bResult;
}
 
template <class T>
inline const T* CTBlock<T>::GetFirst(CIterator& Iterator) const {
  int Bit = m_AllocatedSet.GetFirstSet();
  if (Bit == -1)
    return NULL;
  T * pResult = (m_pMappedRegion + m_nElSize * Bit); 
  Iterator.SetCurrent((void *) Bit);
  return pResult;
}

template <class T>
inline const T* CTBlock<T>::GetLast(CIterator& Iterator) const {
  int Bit = m_AllocatedSet.GetLastSet();
  if (Bit == -1)
    return NULL;
  T * pResult = (m_pMappedRegion + m_nElSize * Bit); 
  Iterator.SetCurrent((void *) Bit);
  return pResult;  
}

template <class T>
inline const T* CTBlock<T>::GetNext(CIterator& Iterator) const {
  int Bit = m_AllocatedSet.GetNextSet((int) Iterator.GetCurrent());
  if (Bit == -1)
    return NULL;
  T * pResult = (m_pMappedRegion + m_nElSize * Bit); 
  Iterator.SetCurrent((void *) Bit);
  return pResult;
}

template <class T>
inline const T* CTBlock<T>::GetPrev(CIterator& Iterator) const {
  int Bit = m_AllocatedSet.GetPrevSet((int) Iterator.GetCurrent());
  if (Bit == -1)
    return NULL;
  T * pResult = (m_pMappedRegion + m_nElSize * Bit); 
  Iterator.SetCurrent((void *) Bit);
  return pResult;  
}

#endif

 
