/*

  Simple Virtual Memory Implementation
  ==================================================
  part of the BASE C++ library
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_VMEMORY_HPP
#define BASE_VMEMORY_HPP

#include <platform/include.hpp>

#include <Object/Object.hpp>
#include <Mutex/Mutex.hpp>
#include <Mutex/RWMutex.hpp>
#include <List/List.hpp>
#include <BitSet/BitSet.hpp>
#include <Swap/ABlock.hpp>

#define V_MAX_CACHE 64
 
class CVMemory : public CObject {
private:
  CABlock<char> *          m_CacheBlocks[V_MAX_CACHE];
  CList< CABlock<char> >   m_vBlocks;
  mutable CRWMutex         m_IrlMutex;
  long                     m_LastMultiple;
  size_t                   m_PageSize;
public:
  static CVMemory          m_VMemory;
  long GetSize(void) const;
  void * VAllocate(size_t Bytes);
  void VFree(void * Pointer);
  CVMemory(void);
  ~CVMemory(void);
};

#endif
