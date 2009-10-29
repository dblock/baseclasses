/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_SSPI_AUTHENTICATION_HPP
#define BASECLASSES_SSPI_AUTHENTICATION_HPP

#include <platform/include.hpp>

#ifdef _WIN32

#include <String/String.hpp>
#include "Win32SecInterface.hpp"
#include "State.hpp"

class CSspiAuthenticator : public CAuthenticator {
    property(CString, Package);
    property(CredHandle, Credentials);
    property(CtxtHandle, Context);    
    property(bool, HaveCredentials);
    property(bool, HaveContext);
    private_property(DWORD, MaxTokenSize);
protected:
    static CWin32SecInterface m_SecInterface;
    void Initialize(void);
    void UnInitialize(void);
    bool Context(const CString& ServerBuffer, CString * AuthResponse);
    bool Chalenge(CString * AuthResponse);
    bool Response(const CString& AuthHeader, CString * AuthResponse);
public:
    virtual bool ChalengeResponse(const CString& AuthHeader, CString * AuthResponse);    
    bool AcquireCredentials(void);
    bool QueryMaxToken(void);
    CSspiAuthenticator(CAuthenticationType AuthType, const CString& Package);
    ~CSspiAuthenticator(void);
};

#endif
#endif
