/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  Virtual Memory Implementation (aka Malloc)

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <platform/include.hpp>

#ifndef BASECLASSES_FILE_MAPPING_HPP
#define BASECLASSES_FILE_MAPPING_HPP

#include <Object/Object.hpp>
#include <Virtual/VirtualMemory.hpp>

typedef struct _CFileView {
#ifdef _WIN32
    HANDLE m_hFile;
    HANDLE m_hMapping;    
    char m_szFilename[MAXPATHLEN];
#endif
#ifdef _UNIX
    size_t m_nSize;
#endif
} CFileView;

class CFileMapping : public CVirtualMemory {    
#ifdef _UNIX
    char m_szFilename[MAXPATHLEN];
    int m_Descriptor;
    long m_Offset;
#endif
protected:
    virtual bool AllocateMemory(size_t Size, void ** pMemory);    
    virtual bool FreeMemory(void * pMemory);
    static bool GetTmpFilename(char szFilename[MAXPATHLEN]);
public:
    bool SetEnabled(bool bEnabled, char * pszFilename = NULL);
    // virtual void GetAccountingInfo(VMPage * pPage, VMAccounting * pAccountingInfo) const;
    virtual void GetAccountingInfo(VMAccounting * pAccountingInfo) const;
    CFileMapping(void);
    static void InitializeSwap(void);
    static void UnInitializeSwap(void);
    static bool GetArgsEnableSwap(int argc, char ** argv, char ** pFilename = NULL);
    virtual void Dispose(void);
};

#endif
