/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "LocalSystem.hpp"

CLocalSystem::CLocalSystem(void) {

}

CLocalSystem::~CLocalSystem(void) {

}

CString CLocalSystem::GetEnv(const CString& Variable) {
    CString Result;
    if (Variable.GetLength()) {
        char * pEnv = getenv(Variable.GetBuffer());
        if (pEnv)
            Result = pEnv;
    }
    return Result;
}

int CLocalSystem::ParseCmdLine(int argc, char ** argv) {
    m_CmdLineOptions.RemoveAll();
    m_CmdLineArguments.RemoveAll();

    if (!argv || !argc)
        return 0;

    CString CurArg;
    char * argPtr;
    for (register int i=1;i<argc;i++) {
        argPtr = argv[i];
        if (argPtr[0] == '-') {
            argPtr++; 
            while (argPtr[0] == '-') 
                argPtr++;
            CurArg = argPtr;
            if (CurArg.GetLength())
                m_CmdLineOptions += CurArg;            
        } else {
            CurArg = argPtr;
            if (CurArg.GetLength())
                m_CmdLineArguments += argv[i];
        }
    }

    return m_CmdLineArguments.GetSize();
}

#ifdef _WIN32

bool CLocalSystem::RegCreateSubTree(HKEY hkRoot, const CString& Path, HKEY * hkResult) {
    HKEY Key = hkRoot; 
    HKEY NewKey; 
    HKEY * pNewKey = hkResult; 
    if (!pNewKey) 
        pNewKey = &NewKey;
    
    CVector<CString> PathVector;
    CString::StrToVector(Path, '\\', &PathVector);    

    for (register int i=0;i<(int) PathVector.GetSize();i++) {
        if (RegOpenKeyA(Key, PathVector[i].GetBuffer(), pNewKey) != ERROR_SUCCESS)
          if (RegCreateKeyA(Key, PathVector[i].GetBuffer(), pNewKey) != ERROR_SUCCESS)
            return false;
        RegCloseKey(Key);
        Key = * pNewKey;                    
    }
    
    if (!hkResult) 
        RegCloseKey(NewKey);

    return true;
}

bool CLocalSystem::RegSetKeyValue(HKEY hkRoot, const CString& Path, const CString& Name, const DWORD dwType, CONST BYTE * lbData, const DWORD cbData) {
  
    HKEY hk = 0;  
    if ((RegOpenKeyA(hkRoot, Path.GetBuffer(), &hk) != ERROR_SUCCESS) || (hk == 0)) {        
      if (!RegCreateSubTree(hkRoot, Path.GetBuffer(), &hk) || (hk == 0))
        return false;      
    }

    bool bResult = true;
    if (RegSetValueExA(hk, Name.GetBuffer(), 0, dwType, lbData, cbData) != ERROR_SUCCESS)
        bResult = false;
    RegCloseKey(hk);

    return bResult;
}

bool CLocalSystem::RegKeyExists(HKEY hkRoot, const CString& Path, const CString& Name) {
    HKEY hk = 0;  
    
    bool bResult = false;

    if ((RegOpenKeyA(hkRoot, Path.GetBuffer(), &hk) == ERROR_SUCCESS) && (hk != 0))
        if (RegQueryValueExA(hk, Name.GetBuffer(), 0, NULL, NULL, NULL) == ERROR_SUCCESS) 
            bResult = true;
        
    RegCloseKey(hk);
    return bResult;
}

bool CLocalSystem::RegGetValueEx(HKEY hkRoot,
                                 const CString& Path,
                                 const CString& Name,
                                 LPDWORD lpType,
                                 LPBYTE lpData,
                                 LPDWORD lpcbData) {
    if (lpData) 
        ZeroMemory(lpData, * lpcbData);
    
    HKEY hk = 0;
    bool bResult = false;
    
    if ((RegOpenKeyA(hkRoot, Path.GetBuffer(), &hk) == ERROR_SUCCESS) && (hk != 0))
        if (RegQueryValueExA(hk, Name.GetBuffer(), 0, lpType, lpData, lpcbData) == ERROR_SUCCESS)
            bResult = true;
        
    if (hk)
        RegCloseKey(hk);
    return bResult;
}

CString CLocalSystem::RegGetString(HKEY hkRoot, const CString& Path, const CString& Name) {
    
    ULONG dwSize = 0;  
    HKEY hk = 0; 
    bool bResult = false;
    CString Result;          
    
    if (RegOpenKeyA(hkRoot, Path.GetBuffer(), &hk) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hk, Name.GetBuffer(), 0, 0, NULL, &dwSize) == ERROR_SUCCESS) {
            if (dwSize > 0) {
                Result.SetLength(dwSize - 1);
                RegQueryValueExA(hk, Name.GetBuffer(), 0, 0, (LPBYTE) Result.GetBuffer(), &dwSize);
                bResult = true;
            }
        }
    }
    
    if (hk)
        RegCloseKey(hk);
    
    return Result;
}

long CLocalSystem::RegGetLong(HKEY hkRoot, const CString& Path, const CString& Name) {
  long lVal = 0;
  DWORD dwType = REG_DWORD;
  DWORD cbData = sizeof(long);
  if (!RegGetValueEx(hkRoot, Path, Name, &dwType, (LPBYTE) &lVal, &cbData))
    lVal = 0;
  return lVal;
}

bool CLocalSystem::RegSetString(HKEY hkRoot, const CString& Path, const CString& Name, const CString& Value) {
    return RegSetKeyValue(hkRoot, 
        Path, 
        Name, 
        REG_SZ, 
        (BYTE *) Value.GetBuffer(), 
        Value.GetLength() * sizeof(char) + 1);
}

bool CLocalSystem::RegSetLong(HKEY hkRoot, const CString& Path, const CString& Name, const long Value) {
    return RegSetKeyValue(hkRoot, 
        Path, 
        Name, 
        REG_DWORD, 
        (BYTE *) &Value, 
        sizeof(long));
}

void CLocalSystem::GetCh(char * pszMessage, bool bDesktopOnly) {
    STARTUPINFO ProcessStartupInfo;
    GetStartupInfo(& ProcessStartupInfo);
    if (!bDesktopOnly || ProcessStartupInfo.dwFlags & STARTF_USESHOWWINDOW) {
        if (pszMessage)
            cout << pszMessage << endl;
        _getch();
    }
}

#endif
