/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  Virtual Memory Implementation (aka Malloc)

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "VirtualMemory.hpp"

#define VM_DEFAULT_PAGESIZE (64 * MBYTE)

CVirtualMemory * CVirtualMemory :: m_pSwap = NULL;

CVirtualMemory :: CVirtualMemory(void) :
    m_bEnabled(vsDisabled),
    m_pVirtualMemory(NULL),
    m_pCacheNextPage(NULL)
{
    m_PageSize = GetPageSize();
    memset(& m_AccountingInfo, 0, sizeof(VMAccounting));
}

CVirtualMemory :: ~CVirtualMemory(void) {
    Dispose();
}

bool CVirtualMemory :: FreePages(void) {
    
    if (m_bEnabled == vsTerminating)
        return true;

    bool bResult = true;
    
    m_VMMutex.StartWriting();

    VMPage * pCurrentPage = m_pVirtualMemory;

    while (pCurrentPage) {
        VMPage * pNextPage = pCurrentPage->m_pNext;
        if (! FreePage(pCurrentPage))
            bResult = false;
        pCurrentPage = pNextPage;
    }

    m_pVirtualMemory = NULL;

    // reset the accounting information
    memset(& m_AccountingInfo, 0, sizeof(VMAccounting));
    
    m_VMMutex.StopWriting();

    return true;
}

//
// pages are allocated in reverse linked list for straightforward deletion
//

bool CVirtualMemory :: AllocatePage(size_t PageSize, VMPage ** pPage) {
    
    assert(pPage);    
    assert(* pPage == NULL);    
    
    if (PageSize < VM_DEFAULT_PAGESIZE)
        PageSize = VM_DEFAULT_PAGESIZE;
    
    // allocate a block of page size
    if (! AllocateMemory(PageSize, (void **) pPage))
        return false;

    // pointer to next page
    (* pPage)->m_pNext = NULL;        
    
    new(& (* pPage)->m_Mutex) CRWMutex;

    VMBlock * pBlock = (* pPage)->GetFirstBlock();
    
    pBlock->m_fUsed = false;
    pBlock->m_fLast = true;
    pBlock->m_pPage = (* pPage);
    
    // size of memory remaining in page is the entire new block
    pBlock->m_nSize = PageSize - sizeof(VMPage) - sizeof(VMBlock);
    
    // update accounting information
    m_AccountingInfo.stTotal += pBlock->m_nSize; // memory actually used
    m_AccountingInfo.stVirtual += PageSize; // memory actually used
    m_AccountingInfo.nBlockCount++;
    
    (* pPage)->m_nMaxBlock = pBlock->m_nSize;    
    (* pPage)->m_pCacheNextBlock = pBlock;

#ifdef _DEBUG
    (* pPage)->m_pLowerBound = (size_t) (* pPage) + PageSize;

    assert((size_t) pBlock + sizeof(VMBlock) + pBlock->m_nSize == (* pPage)->m_pLowerBound);
#endif
    
    return true;
}

void CVirtualMemory :: Dump(void) {

    m_VMMutex.StartReading();

    VMPage * pPage = m_pVirtualMemory;

    while (pPage) {
        // -----------------
        pPage->m_Mutex.StartReading();
        
        cout << "page at " << (long) pPage << endl;
        
        VMBlock * pBlock = pPage->GetFirstBlock();
        
        while (pBlock) {
            
            cout << "block at " << (long) pBlock;
            cout << " of " << pBlock->m_nSize << " bytes";
            cout << ((pBlock->m_fUsed) ? ", used" : ", free");
            cout << endl;
            
            pBlock = pBlock->GetNextBlock();
            
        }
        
        pPage->m_Mutex.StopReading();
        // -----------------
        pPage = pPage->m_pNext;
    }

    m_VMMutex.StopReading();
}

bool CVirtualMemory :: AllocateBlock(VMPage * pPage, size_t Size, VMBlock ** pBlock) {
    assert(pPage);
    assert(pBlock);
    
    bool bResult = false;

    //
    // this will miss when the block of the max size gets allocated
    //

    if (pPage->m_nMaxBlock >= Size) {
        
        pPage->m_Mutex.StartWriting();
        
        if (pPage->m_nMaxBlock >= Size) {
            
            * pBlock = (pPage->m_pCacheNextBlock ? pPage->m_pCacheNextBlock : pPage->GetFirstBlock());

            while (* pBlock) {
                
                // the adequate block must have space for the memory and a new descriptor        
                if (! (* pBlock)->m_fUsed && 
                    (* pBlock)->m_nSize >= Size + sizeof(VMBlock)) {

                    size_t BlockSize = (* pBlock)->m_nSize;
                    bool fLastBlock = (* pBlock)->m_fLast;
                    
                    (* pBlock)->m_fUsed = true;
                    (* pBlock)->m_nSize = Size;
                    (* pBlock)->m_fLast = false;

                    VMBlock * pNextBlock = (* pBlock)->GetNextBlock();
                    
                    // next block gets a new descriptor
                    pNextBlock->m_fUsed = false;
                    pNextBlock->m_nSize = BlockSize - Size - sizeof(_VMBlock);
                    pNextBlock->m_fLast = fLastBlock;
                    pNextBlock->m_pPage = (* pBlock)->m_pPage;

#ifdef _DEBUG
                    // check that still within page bounds
                    assert((size_t) pNextBlock < (* pBlock)->m_pPage->m_pLowerBound);

                    // last block aligned with lower page boundary
                    if (pNextBlock->m_fLast) {
                        assert((size_t) pNextBlock + sizeof(VMBlock) + pNextBlock->m_nSize == (* pBlock)->m_pPage->m_pLowerBound);
                    }
#endif

                    // reset the cache
                    if (pPage->m_nMaxBlock == BlockSize) {
                        pPage->m_nMaxBlock = pNextBlock->m_nSize;                        
                    }
                    
                    pPage->m_pCacheNextBlock = pNextBlock;

                    m_pCacheNextPage = pPage;
    
                    // update accounting information
                    m_AccountingInfo.stUsed += Size; // memory actually used
                    m_AccountingInfo.stTotal -= sizeof(VMBlock); // total virtual memory available
                    m_AccountingInfo.nBlockCount++; // current block was split
                    
                    bResult = true;
                    break;
                }
                
                (* pBlock) = (* pBlock)->GetNextBlock();
            }
        }

        pPage->m_Mutex.StopWriting();
    }

    return bResult;
}

bool CVirtualMemory :: AllocateBlock(size_t Size, VMBlock ** pBlock) {
    assert(pBlock);
    assert(* pBlock == NULL);
    
    bool bResult = false;

    m_VMMutex.StartReading();

    bResult = AllocateExistingBlock(Size, pBlock);
        
    m_VMMutex.StopReading();

    if (bResult)
        return true;

    // create a new page, avoid creating many pages from many threads, hence this is locked

    size_t NewPageSize = GetBlockSize(Size);
    
    VMPage * pPage = NULL;

    // cout << (long) GetCurrentThreadId() << "Failed to allocate a block of " << (long) Size << " bytes." << endl;

    m_VMMutex.StartWriting();    

    m_pCacheNextPage = NULL;

    // cout << (long) GetCurrentThreadId() << "Attempting to lock-allocate a block of " << (long) Size << " bytes." << endl;

    bResult = AllocateExistingBlock(Size, pBlock);

    // cout << (long) GetCurrentThreadId() << "Allocation result is " << (bResult ? "true" : "false") << endl;

    if (! bResult) {

        // cout << (long) GetCurrentThreadId() << "Allocating page of " << NewPageSize << " bytes." << endl;
        
        if (AllocatePage(NewPageSize, & pPage)) {
            
            // allocate a block on the new page
            
            pPage->m_pNext = m_pVirtualMemory;
            m_pVirtualMemory = pPage;
            m_pCacheNextPage = pPage;
            
            bResult = AllocateBlock(pPage, Size, pBlock);
            
            if (! bResult) {
                cerr << "VirtualMemory: AllocateBlock failed for " << Size << " bytes on a new page." << endl;
                assert(bResult);
                exit(-1);
            }
        }
    } 

    m_VMMutex.StopWriting();

    return bResult;
}

void CVirtualMemory :: Free(void * pMem) {

    if (m_bEnabled == vsTerminating)
        return;

    if (m_bEnabled != vsEnabled) {
        free(pMem);
        return;
    }

    VMBlock * pBlock = GetBlock(pMem);
    VMPage * pPage = pBlock->m_pPage;
    
    assert(pBlock->m_fUsed);

    pPage->m_Mutex.StartWriting();

    // update accounting information
    m_AccountingInfo.stUsed -= pBlock->m_nSize; // memory actually used
    
    pBlock->m_fUsed = false;

    if (pPage->m_nMaxBlock < pBlock->m_nSize) {
        pPage->m_nMaxBlock = pBlock->m_nSize;                
    }

    // always reset, otherwise after merge the block may not exist
    pPage->m_pCacheNextBlock = pBlock;
    m_pCacheNextPage = pPage;

    VMBlock * pNextBlock = NULL;

    while (! pBlock->m_fLast && ! (pNextBlock = pBlock->GetNextBlock())->m_fUsed) {
        // can link with next block               
        pBlock->m_nSize += sizeof(_VMBlock);
        pBlock->m_nSize += pNextBlock->m_nSize;
        pBlock->m_fLast = pNextBlock->m_fLast;
    
        // update accounting information
        m_AccountingInfo.stTotal += sizeof(VMBlock); // total virtual memory available
        m_AccountingInfo.nBlockCount--; // current block was split    
    }

#ifdef _DEBUG
    // check that still within page bounds
    assert((size_t) pBlock < pBlock->m_pPage->m_pLowerBound);
    
    // last block aligned with lower page boundary
    if (pBlock->m_fLast) {
        assert((size_t) pBlock + sizeof(VMBlock) + pBlock->m_nSize == pBlock->m_pPage->m_pLowerBound);
    }
#endif

    pPage->m_Mutex.StopWriting();
    
}

bool CVirtualMemory :: FindPage(VMBlock * pBlock, VMPage ** pPage) {
    assert(pPage);
    assert(* pPage == NULL);

    assert(m_pVirtualMemory);

    m_VMMutex.StartReading();

    VMPage * pCurrentPage = m_pVirtualMemory;

    while (pCurrentPage) {

        if (((size_t) pBlock > (size_t) pCurrentPage) && 
            ((size_t) pBlock - (size_t)pCurrentPage < (size_t) pBlock - (size_t) (* pPage))) {
            * pPage = pCurrentPage;
        }

        pCurrentPage = pCurrentPage->m_pNext;
    }

    m_VMMutex.StopReading();

    assert(* pPage);

    return true;
}

// void CVirtualMemory :: GetAccountingInfo(VMPage * pPage, VMAccounting * pAccountingInfo) const {
//     pAccountingInfo->stVirtual += sizeof(VMPage);
    
//     pPage->m_Mutex.StartReading();
    
//     VMBlock * pBlock = pPage->GetFirstBlock();

//     bool bFreeBusy = false;
    
//     while (pBlock) {

//         if ((pBlock->m_fUsed != bFreeBusy) || (! pBlock->m_fUsed && ! bFreeBusy))
//             pAccountingInfo->nFragCount++;

//         bFreeBusy = pBlock->m_fUsed;

//         pAccountingInfo->nBlockCount++;
//         pAccountingInfo->stTotal += pBlock->m_nSize;
//         pAccountingInfo->stVirtual += pBlock->m_nSize;
//         pAccountingInfo->stVirtual += sizeof(VMBlock);
//         pAccountingInfo->stUsed += ((pBlock->m_fUsed) ? pBlock->m_nSize : 0);
//         pBlock = pBlock->GetNextBlock();

//     }
    
//     pPage->m_Mutex.StopReading();    
// }

void CVirtualMemory :: GetAccountingInfo(VMAccounting * pAccountingInfo) const {
    assert(pAccountingInfo);
    
    m_VMMutex.StartReading();
    
    memcpy(pAccountingInfo, & m_AccountingInfo, sizeof(VMAccounting));
    
    m_VMMutex.StopReading();
    
//    memset(pAccountingInfo, 0, sizeof(VMAccounting));
    
//     m_VMMutex.StartReading();
    
//     VMPage * pCurrentPage = m_pVirtualMemory;
    
//     while (pCurrentPage) {
//         GetAccountingInfo(pCurrentPage, pAccountingInfo);
//         pCurrentPage = pCurrentPage->m_pNext;
//     }

//     m_VMMutex.StopReading();
}

