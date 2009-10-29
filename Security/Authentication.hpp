/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_AUTHENTICATION_HPP
#define BASECLASSES_AUTHENTICATION_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

#include "Win32SecInterface.hpp"
#include "State.hpp"

class CAuthenticator {
    property(CAuthenticationType, AuthType);
    property(CString, Username);
    property(CString, Password);
    property(CString, Domain);
public:
    CAuthenticator(CAuthenticationType AuthType);
    virtual ~CAuthenticator(void);
    virtual bool ChalengeResponse(const CString& AuthHeader, CString * AuthResponse) = 0;
};

#endif
