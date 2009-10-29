/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Serge Huber - huber@xo3.com
    and Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Object.hpp"
#include <Virtual/VirtualMemory.hpp>

#ifdef ROCKALL_HEAPS
 bool g_RockallInitialized = false;
#endif

void * CObject :: __object_new(size_t stAllocateBlock) {
    if (CVirtualMemory::m_pSwap)
        return CVirtualMemory::m_pSwap->Allocate(stAllocateBlock);
    else return malloc(stAllocateBlock);
}

void CObject :: __object_delete(void * pvMem) {
    if (CVirtualMemory::m_pSwap)
        CVirtualMemory::m_pSwap->Free(pvMem);
    else free(pvMem);
}

#ifdef _UNIX
void * operator new( size_t stAllocateBlock ) {
    return CObject :: __object_new(stAllocateBlock);
}

void * operator new[]( size_t stAllocateBlock ) {
    return CObject :: __object_new(stAllocateBlock);
}

void operator delete[](void * pvMem) {
    CObject :: __object_delete(pvMem);
}

void operator delete(void * pvMem) {
    CObject :: __object_delete(pvMem);
}
#endif
