/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  Virtual Memory Implementation (aka Malloc)

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <platform/include.hpp>

#ifndef BASECLASSES_VIRTUAL_MEMORY_HPP
#define BASECLASSES_VIRTUAL_MEMORY_HPP

#include <Object/Object.hpp>
#include <Mutex/RWMutex.hpp>

//
// an allocated block
//

struct _VMPage;

typedef struct _VMBlock {
    size_t m_nSize; // size of block
    bool m_fUsed; // used or free
    bool m_fLast; // last block in page (to support arbitrary page size)
    _VMPage * m_pPage; // page pointer
    // allocated memory always starts after sizeof(_VMBlock)
    // next block is always after sizeof(_VMBlock) + m_nSize
    inline _VMBlock * GetNextBlock(void) const { return m_fLast ? NULL : (_VMBlock *) ((size_t) this + sizeof(_VMBlock) + m_nSize); }
    inline void * GetMemory(void) const { return (void *) ((size_t) this + sizeof(_VMBlock)); }
} VMBlock;

//
// an allocated page
//

typedef struct _VMPage {
    _VMPage * m_pNext; // next page
    CRWMutex m_Mutex;
    size_t m_nMaxBlock;
    _VMBlock * m_pCacheNextBlock;
#ifdef _DEBUG
    size_t m_pLowerBound;
#endif
    // memory always starts after sizeof(_VMPage)
    inline VMBlock * GetFirstBlock(void) const { return (VMBlock *) ((size_t) this + sizeof(_VMPage)); }
} VMPage;

//
// accounting information
//

typedef struct _VMAccounting {
    size_t stUsed;                  // memory used
    size_t stTotal;                 // total virtual memory usable (pBlock->m_nSize)
    size_t stVirtual;               // total virtual memory used (pBlock->m_nSize + sizeof(VMBlock)
    unsigned int nFragCount;        // fragmentation counter
    unsigned long nBlockCount;      // number of blocks
} VMAccounting;

typedef enum { vsDisabled, vsEnabled, vsTerminating } CVirtualState;

class CVirtualMemory {
protected:
    CVirtualState m_bEnabled;
    size_t m_PageSize;
    VMPage * m_pVirtualMemory;
    mutable CRWMutex m_VMMutex;
    VMPage * m_pCacheNextPage;
    VMBlock * m_pCacheNextBlock;
    VMAccounting m_AccountingInfo;
protected:
    size_t GetBlockSize(size_t Size) const;
    bool AllocatePage(size_t PageSize, VMPage ** pPage);
    bool AllocateBlock(size_t Size, VMBlock ** pBlock);
    bool AllocateExistingBlock(size_t Size, VMBlock ** pBlock);
    bool FreePage(VMPage * pPage);
    bool FreePages(void);
    bool AllocateBlock(VMPage * pPage, size_t Size, VMBlock ** pBlock);
    bool FindPage(VMBlock * pBlock, VMPage ** pPage);
    inline static VMBlock * GetBlock(void * pMem) { return (VMBlock *) ((size_t) pMem - sizeof(_VMBlock)); }
protected:
    virtual bool AllocateMemory(size_t Size, void ** pMemory);    
    virtual bool FreeMemory(void * pMemory);
public:
    static CVirtualMemory * m_pSwap;
public:
    // generic system routines
    static size_t GetPageSize(void);     
    // dump routines
    void Dump(void);
    CVirtualMemory(void);
    virtual ~CVirtualMemory(void);
    // allocation routines
    void * Allocate(size_t Size);
    void Free(void * pMem);
    // virtual void GetAccountingInfo(VMPage * pPage, VMAccounting * pAccountingInfo) const;
    virtual void GetAccountingInfo(VMAccounting * pAccountingInfo) const;
    bool SetEnabled(bool bEnabled);    
    inline void SetState(CVirtualState State) { m_bEnabled = State; }
    virtual void Dispose(void);
};

inline bool CVirtualMemory :: SetEnabled(bool bEnabled) {
    m_bEnabled = bEnabled ? vsEnabled : vsDisabled;
    return true;
}

inline void CVirtualMemory :: Dispose(void) {
    
    FreePages();
}

inline bool CVirtualMemory :: AllocateMemory(size_t Size, void ** pMemory) {
    assert(pMemory);
    assert(* pMemory == NULL);
    assert(Size);

    * pMemory = malloc(Size);
    
    return (* pMemory) ? true : false;
}

inline bool CVirtualMemory :: FreeMemory(void * pMemory) {
    assert(pMemory);

    free(pMemory);

    return true;
}

inline bool CVirtualMemory :: FreePage(VMPage * pPage) {
    assert(pPage);

    pPage->m_Mutex.~CRWMutex();

    if (! FreeMemory((void *) pPage))
        return false;
    
    return true;
}

inline size_t CVirtualMemory :: GetPageSize(void) {
#ifdef _UNIX
    return (size_t) getpagesize();
#endif
#ifdef _WIN32
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    return (size_t) SystemInfo.dwAllocationGranularity;
#endif    
}

inline bool CVirtualMemory :: AllocateExistingBlock(size_t Size, VMBlock ** pBlock) {
    bool bResult = false;
    
    VMPage * pCurrentPage = m_pCacheNextPage ? m_pCacheNextPage : m_pVirtualMemory;

    while (pCurrentPage) {
        
        if (AllocateBlock(pCurrentPage, Size, pBlock)) {
            bResult = true;
            break;
        }
        
        pCurrentPage = pCurrentPage->m_pNext;
    }

    return bResult;
}

inline size_t CVirtualMemory :: GetBlockSize(size_t Size) const {
    
    size_t NewPageSize = m_PageSize;

    while (NewPageSize - sizeof(VMPage) - sizeof(VMBlock) < Size)
        NewPageSize += m_PageSize;

    return NewPageSize;    
}

inline void * CVirtualMemory :: Allocate(size_t Size) {

    if (m_bEnabled != vsEnabled)
        return malloc(Size);

    VMBlock * pBlock = NULL;

    if (! AllocateBlock(Size, & pBlock)) {
        assert(false);
        return NULL;
    }

    return pBlock->GetMemory();  
}
    
#endif
