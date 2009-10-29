/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_SECURITY_INTERFACE_HPP
#define BASECLASSES_SECURITY_INTERFACE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

#ifdef _WIN32

typedef struct _SEC_FUNC {
    FREE_CREDENTIALS_HANDLE_FN pFreeCredentialsHandle;
    ACQUIRE_CREDENTIALS_HANDLE_FN pAcquireCredentialsHandle;
    QUERY_SECURITY_PACKAGE_INFO_FN pQuerySecurityPackageInfo;   // A
    FREE_CONTEXT_BUFFER_FN pFreeContextBuffer;
    INITIALIZE_SECURITY_CONTEXT_FN pInitializeSecurityContext;  // A
    COMPLETE_AUTH_TOKEN_FN pCompleteAuthToken;
    ENUMERATE_SECURITY_PACKAGES_FN pEnumerateSecurityPackages;  // A
    DELETE_SECURITY_CONTEXT_FN pDeleteSecurityContext;
} SEC_FUNC;

// the security interface

class CWin32SecInterface {
    property(SEC_FUNC, SfProcs);
    property(CString, Library);
    property(HINSTANCE, SecurityLibrary);
public:
    bool DetectSecurityDll(void);
    virtual bool Initialize(void);
    virtual bool UnInitialize(void);
    CWin32SecInterface(void);
    virtual ~CWin32SecInterface(void);    
    // detect if CredHandle is valid
    inline static void SetInvalidCredHandle(CredHandle * phCred);
    inline static bool IsInvalidCredHandle(CredHandle hCred);
    // detect if CtxtHandle is valid
    inline static void SetInvalidCtxtHandle(CtxtHandle * hCtxt);
    inline static bool IsInvalidCtxtHandle(CtxtHandle hCtxt);
};

inline void CWin32SecInterface :: SetInvalidCredHandle(CredHandle * phCred) {
    memset((LPVOID) phCred, 0xFF, sizeof(CredHandle));
}

inline bool CWin32SecInterface :: IsInvalidCredHandle(CredHandle hCred) {
    for (int i=0; i < sizeof(CredHandle); i++) {
		if (((PBYTE) & hCred)[i] != 0xFF)
			return false;
	}
	return true;
}

inline void CWin32SecInterface :: SetInvalidCtxtHandle(CtxtHandle * phCtxt) {
    memset((LPVOID) phCtxt, 0xFF, sizeof(CtxtHandle));
}

inline bool CWin32SecInterface :: IsInvalidCtxtHandle(CtxtHandle hCtxt) {
    for(int i=0; i < sizeof(CredHandle); i++) {
		if (((PBYTE) &hCtxt)[i] != 0xFF)
			return false;
	}
	return true;
}

#endif
#endif

