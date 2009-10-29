/*

  Simple Virtual Memory Implementation
  ==================================================
  part of the BASE C++ library
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TMEMORY_HPP
#define BASE_TMEMORY_HPP

#include <platform/include.hpp>

#include <Object/Object.hpp>
#include <Mutex/Mutex.hpp>
#include <Mutex/RWMutex.hpp>
#include <List/List.hpp>
#include <BitSet/BitSet.hpp>
#include <Swap/ABlock.hpp>

template <class T> 
class CTMemory : public CObject {
private:  
  CList< CABlock<T> >      m_vBlocks;
  mutable CRWMutex         m_IrlMutex;  
  size_t                   m_PageSize;
public:  
  long GetSize(void) const;
  T * Allocate(void);
  void Free(T * Pointer);
  CTMemory(void);
  ~CTMemory(void);
  /* next/previous iterator support */
  inline const T * GetFirst(CIterator& BlockIterator, CIterator& ChunkIterator) const;
  inline const T * GetNext(CIterator& BlockIterator, CIterator& ChunkIterator) const;
  inline const T * GetLast(CIterator& BlockIterator, CIterator& ChunkIterator) const;
  inline const T * GetPrev(CIterator& BlockIterator, CIterator& ChunkIterator) const;
};

template <class T>
inline const T * CTMemory<T>::GetFirst(CIterator& BlockIterator, CIterator& ChunkIterator) const {
  m_IrlMutex.StartReading();

  const CABlock<T> * FirstBlock;
  const T * FirstChunk;
  
  do {
    // get the first block
    FirstBlock = m_vBlocks.GetFirst(BlockIterator);

    // if there're no blocks, done
    if (!FirstBlock)
      break;
    
    // get the chunk from this block
    FirstChunk = FirstBlock->GetFirst(ChunkIterator);

    // when a chunk is present, then we're happy, otherwise, continue with the next block
    if (FirstChunk)
      return FirstChunk; // mutex is not released on purpose
    
  } while (FirstBlock);    
  
  m_IrlMutex.StopReading();
  return NULL;
}

template <class T>
inline const T * CTMemory<T>::GetLast(CIterator& BlockIterator, CIterator& ChunkIterator) const {
  m_IrlMutex.StartReading();

  const CABlock<T> * LastBlock;
  const T * LastChunk;
  
  do {
    // get the first block
    LastBlock = m_vBlocks.GetLast(BlockIterator);

    // if there're no blocks, done
    if (!LastBlock)
      break;
    
    // get the chunk from this block
    LastChunk = LastBlock->GetLast(ChunkIterator);

    // when a chunk is present, then we're happy, otherwise, continue with the next block
    if (LastChunk)
      return LastChunk; // mutex is not released on purpose
    
  } while (LastChunk);    
  
  m_IrlMutex.StopReading();
  return NULL;
}

template <class T>
inline const T * CTMemory<T>::GetNext(CIterator& BlockIterator, CIterator& ChunkIterator) const {
  
  const CABlock<T> * CurrentBlock = (CABlock<T> *) BlockIterator.GetCurrent();  
  assert(CurrentBlock);  

  // get the next chunk from this block
  const T * NewChunk = CurrentBlock->GetNext(ChunkIterator);
  
  while (CurrentBlock) {
    
    // when a chunk is present, then we're happy, otherwise, continue with the next block
    if (NewChunk)
      return NewChunk; // mutex is not released on purpose
   
    CurrentBlock = m_vBlocks.GetNext(BlockIterator); 
    if (CurrentBlock)
      NewChunk = CurrentBlock->GetFirst(ChunkIterator);
  }

  // if we are out of blocks and no new chunk has been found  
  m_IrlMutex.StopReading();
  return NULL;
}

template <class T>
inline const T * CTMemory<T>::GetPrev(CIterator& BlockIterator, CIterator& ChunkIterator) const {
  
  const CABlock<T> * CurrentBlock = (CABlock<T> *) BlockIterator.GetCurrent();  
  assert(CurrentBlock);  

  // get the next chunk from this block
  const T * NewChunk = CurrentBlock->GetPrev(ChunkIterator);
  
  while (CurrentBlock) {
    
    // when a chunk is present, then we're happy, otherwise, continue with the next block
    if (NewChunk)
      return NewChunk; // mutex is not released on purpose
   
    CurrentBlock = m_vBlocks.GetPrev(BlockIterator); 
    if (CurrentBlock)
      NewChunk = CurrentBlock->GetLast(ChunkIterator);
  }
  
  // if we are out of blocks and no new chunk has been found  
  m_IrlMutex.StopReading();
  return NULL;
}

#endif
