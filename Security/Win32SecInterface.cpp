/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "Win32SecInterface.hpp"

#ifdef _WIN32

CWin32SecInterface :: CWin32SecInterface(void) :
    m_SecurityLibrary(NULL) {

    DetectSecurityDll();
    Initialize();
}

CWin32SecInterface :: ~CWin32SecInterface(void) {

    UnInitialize();
}

bool CWin32SecInterface :: Initialize(void) {
    
    if (m_Library.GetLength() == 0)
        return false;

    //
    //  Load Security DLL
    //

    m_SecurityLibrary = LoadLibrary (m_Library.GetBuffer());
    if (m_SecurityLibrary == NULL)
        return false;

    // Get all entry points we care about

    m_SfProcs.pFreeCredentialsHandle
            = (FREE_CREDENTIALS_HANDLE_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "FreeCredentialsHandle" );

    m_SfProcs.pQuerySecurityPackageInfo
            = (QUERY_SECURITY_PACKAGE_INFO_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "QuerySecurityPackageInfoA" );

    m_SfProcs.pAcquireCredentialsHandle
            = (ACQUIRE_CREDENTIALS_HANDLE_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "AcquireCredentialsHandleA" );

    m_SfProcs.pFreeContextBuffer
            = (FREE_CONTEXT_BUFFER_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "FreeContextBuffer" );

    m_SfProcs.pInitializeSecurityContext
            = (INITIALIZE_SECURITY_CONTEXT_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "InitializeSecurityContextA" );

    m_SfProcs.pCompleteAuthToken
            = (COMPLETE_AUTH_TOKEN_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "CompleteAuthToken" );

    m_SfProcs.pEnumerateSecurityPackages
            = (ENUMERATE_SECURITY_PACKAGES_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "EnumerateSecurityPackagesA" );

    m_SfProcs.pDeleteSecurityContext
            = (DELETE_SECURITY_CONTEXT_FN) GetProcAddress(
                    m_SecurityLibrary,
                    "DeleteSecurityContext" );

    if ( m_SfProcs.pFreeCredentialsHandle == NULL
            || m_SfProcs.pQuerySecurityPackageInfo == NULL
            || m_SfProcs.pAcquireCredentialsHandle == NULL
            || m_SfProcs.pFreeContextBuffer == NULL
            || m_SfProcs.pInitializeSecurityContext == NULL
            || m_SfProcs.pEnumerateSecurityPackages == NULL )
    {
        return false;
    }

    return true;
}

bool CWin32SecInterface :: UnInitialize(void) {
    // uninitialize the security interface

	if (m_SecurityLibrary != NULL)
        :: FreeLibrary(m_SecurityLibrary);

    return true;
}

bool CWin32SecInterface :: DetectSecurityDll(void) {
    OSVERSIONINFO VerInfo;

    //
    //  Find out which security DLL to use, depending on
    //  whether we are on NT or Win95
    //

    VerInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (! GetVersionEx (&VerInfo))   // If this fails, something has gone wrong
        return false;
    
    switch(VerInfo.dwPlatformId) {
    case VER_PLATFORM_WIN32_NT:
        m_Library = "security.dll";
        break;
    case VER_PLATFORM_WIN32_WINDOWS:
        m_Library = "secur32.dll";
        break;
    default:
        return false;
    }
    
    return true;
}

#endif
