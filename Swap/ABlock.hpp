/*

  Simple Virtual Memory Implementation
  ==================================================
  part of the BASE C++ library
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_ABLOCK_HPP
#define BASE_ABLOCK_HPP

#include <platform/include.hpp>
#include <Swap/TBlock.hpp>

template <class T>
class CABlock : public CTBlock<T> {    
public:    
    CABlock(size_t nElSize = 0);
    void Initialize(size_t MappedRegionSize);
    virtual ~CABlock(void);
};

#endif
