/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_LOCAL_SYSTEM_HPP
#define BASE_LOCAL_SYSTEM_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>

class CLocalSystem : public CObject {
    property(CVector<CString>, CmdLineArguments);
    property(CVector<CString>, CmdLineOptions);
public:
    CLocalSystem(void);
    virtual ~CLocalSystem(void);
public:
    static CString GetEnv(const CString& Variable);
    int ParseCmdLine(int argc, char ** argv);
#ifdef _WIN32
public:
    // create a tree of registry keys
    static bool RegCreateSubTree(HKEY hkRoot, const CString& Path, HKEY * hkResult = NULL);
    static bool RegSetKeyValue(HKEY hkRoot, const CString& Path, const CString& Name, const DWORD dwType, CONST BYTE * lbData, const DWORD cbData);
    static bool RegKeyExists(HKEY hkRoot, const CString& Path, const CString& Name);
    static bool RegGetValueEx(HKEY hkRoot, const CString& Path, const CString& Name, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);    
    static CString RegGetString(HKEY hkRoot, const CString& Path, const CString& Name);
    static bool RegSetString(HKEY hkRoot, const CString& Path, const CString& Name, const CString& Value);
    static long RegGetLong(HKEY hkRoot, const CString& Path, const CString& Name);
    static bool RegSetLong(HKEY hkRoot, const CString& Path, const CString& Name, const long Value);
    static void GetCh(char * pszMessage = "Press any key to continue...", bool bDesktopOnly = true);
#endif
};

#endif
