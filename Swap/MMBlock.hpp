/*

  MMAP Simple Virtual Memory Implementation
  ==================================================
  part of the Alkaline Search Engine
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_MMBLOCK_HPP
#define BASE_MMBLOCK_HPP

#include <platform/include.hpp>
#include <Swap/VBlock.hpp>

class CMMSwap;

class CMMBlock : public CVBlock {
  friend class CMMSwap;
private:
  off_t          m_Offset;
  CMMBlock *     m_pNext;  
public:
  CMMBlock(size_t nElSize = 0);
#ifdef _WIN32
  off_t Initialize(off_t Offset, 
      HANDLE Descriptor, 
      HANDLE Mapping, 
      size_t MappedRegionSize);
#endif
#ifdef _UNIX
  off_t Initialize(
      off_t Offset,
      int Descriptor, 
      size_t MappedRegionSize);
#endif
  virtual ~CMMBlock(void);  
};

#endif
