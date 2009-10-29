/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  Virtual Memory Implementation (aka Malloc)

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "FileMapping.hpp"

void CFileMapping :: InitializeSwap(void) {
    //assert(! CVirtualMemory::m_pSwap);
    void * pMem = malloc(sizeof(CFileMapping));
    CVirtualMemory::m_pSwap = new(pMem) CFileMapping;
}

void CFileMapping :: UnInitializeSwap(void) {
    ((CFileMapping *) CVirtualMemory::m_pSwap)->Dispose();
    free(CVirtualMemory::m_pSwap);
    CVirtualMemory::m_pSwap = NULL;
}


CFileMapping :: CFileMapping(void) {
#ifdef _UNIX    
    m_Descriptor = -1;
    m_Offset = 0;
#endif
}

bool CFileMapping :: AllocateMemory(size_t Size, void ** pMemory) {

    CFileView TmpFileView;

#ifdef _WIN32

    if (! GetTmpFilename(TmpFileView.m_szFilename))
        return false;

    TmpFileView.m_hFile = CreateFile(
        TmpFileView.m_szFilename, 
        GENERIC_ALL,
        0, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE,
        0);

    if (! TmpFileView.m_hFile) {
        cerr << "CFileMapping: Error in GetTmpHandleSz (" << TmpFileView.m_szFilename << ")" << endl;
        CObject :: ShowLastError();
        CHandler :: Terminate(GetLastError());
    }

    if (! SetFilePointer(
        TmpFileView.m_hFile,
        Size + sizeof(CFileView),
        NULL,
        SEEK_SET)) {
        cerr << "CFileMapping: Error in SetFilePointer (" << TmpFileView.m_szFilename << ")" << endl;
        CObject :: ShowLastError();
        CHandler :: Terminate(GetLastError());
    }

    if (! SetEndOfFile(TmpFileView.m_hFile)) {
        cerr << "CFileMapping: Error in SetEndOfFile (" << TmpFileView.m_szFilename << ")" << endl;
        CObject :: ShowLastError();
        CHandler :: Terminate(GetLastError());
    }

    TmpFileView.m_hMapping = CreateFileMapping(
        TmpFileView.m_hFile,
        NULL,
        PAGE_READWRITE, 
        0, 
        Size + sizeof(CFileView), 
        NULL);

    if (! TmpFileView.m_hMapping) {
        cerr << "CFileMapping: Error in CreateFileMapping (" << Size << ")" << endl;
        CObject :: ShowLastError();
        CHandler :: Terminate(GetLastError());
    }

    LPVOID pMappedRegion = (LPVOID) MapViewOfFile(
        TmpFileView.m_hMapping, 
        FILE_MAP_ALL_ACCESS, 
        0, 
        0, 
        Size + sizeof(CFileView));

    if (! pMappedRegion) {
        cerr << "CFileMapping: Error in MapViewOfFile" << endl;
        CObject::ShowLastError();
        CHandler :: Terminate(GetLastError());
    }
#endif

#ifdef _UNIX
    
    TmpFileView.m_nSize = Size + sizeof(CFileView);    

    // this size must be aligned properly
    TmpFileView.m_nSize = (TmpFileView.m_nSize / m_PageSize + 1) * m_PageSize;
    
    // calculate new offset
    long nNewOffset = m_Offset + TmpFileView.m_nSize;
    // resize the file
    base_seek(m_Descriptor, nNewOffset, SEEK_SET);
    // write a byte to extend the file size
    base_write(m_Descriptor, (const char *) "\0", 1);
    
    // map the region
    void * pMappedRegion = (void *) mmap(
        0, 
        TmpFileView.m_nSize, 
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE,
        m_Descriptor, 
        m_Offset);
    
    if (pMappedRegion == (void *) -1) {
        cerr << "CFileMapping :: mmap failed." << endl;
        perror("CFileMapping");
        CHandler :: Terminate(-1);
    }

    m_Offset = nNewOffset;
    
#endif

    memcpy(pMappedRegion, & TmpFileView, sizeof(CFileView));

    * pMemory = (void *) ((size_t) pMappedRegion + sizeof(CFileView));
    
    // update accounting info
    CVirtualMemory :: m_AccountingInfo.stVirtual += sizeof(CFileView);
    
    return true;    
}

bool CFileMapping :: FreeMemory(void * pMemory) {

    CFileView * pFileView = (CFileView *) ((size_t) pMemory - sizeof(CFileView));
    
#ifdef _WIN32
    CFileView TmpFileView;

    memcpy(& TmpFileView, pFileView, sizeof(CFileView));

    UnmapViewOfFile((LPCVOID) pFileView);
    CloseHandle(TmpFileView.m_hMapping);
    CloseHandle(TmpFileView.m_hFile);
#endif

#ifdef _UNIX
    // if (m_bEnabled != vsTerminating) {
    if (munmap((caddr_t) pFileView, pFileView->m_nSize) == -1) {
        cerr << "CFileMapping: Error in munmap" << endl;
        perror("CFileMapping");
    }
    // }
#endif

    return true;
}

bool CFileMapping :: GetTmpFilename(char szFilename[MAXPATHLEN]) {    
#ifdef _WIN32
    static CAtomic FileHandles;    
    char szTmpPath[MAX_PATH];
    ZeroMemory(szTmpPath, sizeof(szTmpPath));
    GetTempPath(MAX_PATH, (char *) szTmpPath);    
    if (base_strlen((char *) szTmpPath)) {
        base_snprintf(szFilename, MAXPATHLEN, "%s~blib-%d-%d.swp", szTmpPath, base_getpid(), FileHandles.Inc());
    } else base_snprintf(szFilename, MAXPATHLEN, "%s", tmpnam(NULL));
    return true;
#endif
#ifdef _UNIX
    assert(0);
    return false;
#endif
}

bool CFileMapping :: SetEnabled(bool bEnabled, char * pszFilename) {    

#ifdef _WIN32
    if (pszFilename) {
        cerr << "CFileMapping: file name specification for the swap file ignored under Windows NT." << endl;
    }
#endif

#ifdef _UNIX
    if (pszFilename) {
        int nLen = strlen(pszFilename);
        if (nLen >= MAXPATHLEN) {
            cerr << "CFileMapping: temporary file name is too long (" << pszFilename << ")" << endl;
            CHandler :: Terminate(-1);
        }
        memcpy(m_szFilename, pszFilename, (nLen + 1) * sizeof(char));
    } else {
        base_snprintf(m_szFilename, MAXPATHLEN, "/tmp/blib-%d.swp", base_getpid());
    }
    
    m_Descriptor = base_open((const char *) m_szFilename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    
    if (m_Descriptor == -1) {
        cerr << "CFileMapping: Error in file open (" << m_szFilename << ")" << endl;        
        perror("CFileMapping");
        CHandler :: Terminate(-1);        
    }
    
#endif

    CVirtualMemory :: SetEnabled(bEnabled);
    return true;
}

// void CFileMapping :: GetAccountingInfo(VMPage * pPage, VMAccounting * pAccountingInfo) const {
//     CVirtualMemory :: GetAccountingInfo(pPage, pAccountingInfo);
//     pAccountingInfo->stTotal += sizeof(CFileView);
// }

void CFileMapping :: GetAccountingInfo(VMAccounting * pAccountingInfo) const {
    CVirtualMemory :: GetAccountingInfo(pAccountingInfo);
}

bool CFileMapping :: GetArgsEnableSwap(int argc, char ** argv, char ** pFilename) {
    bool bResult = false;

    for (register int i=1;i<argc;i++) {
        char * pArg = argv[i];
        while ((* pArg == '-') || (* pArg == '/'))
            pArg++;
        
        char * pEq = strchr(pArg, '=');
        if (pEq != NULL) {
            * pEq = 0;
            pEq++;
        }
        
        if (! strcmp(pArg, "enableswap")) {
            bResult = true;
            if (pFilename)
                * pFilename = pEq;            
        }
        
        if (pEq) * (pEq - 1) = '=';
    }

    return bResult;
}

void CFileMapping :: Dispose(void) {

    CVirtualMemory :: Dispose();
    
#ifdef _UNIX
    if (m_Descriptor >= 0) {
        base_close(m_Descriptor);
        base_unlink(m_szFilename);
    }
#endif
}
