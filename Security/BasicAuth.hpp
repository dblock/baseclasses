/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_BASIC_AUTHENTICATION_HPP
#define BASECLASSES_BASIC_AUTHENTICATION_HPP

#include <platform/include.hpp>
#include "Authentication.hpp"

class CBasicAuthenticator : public CAuthenticator {
public:
    CBasicAuthenticator(void);
    virtual ~CBasicAuthenticator(void);
    virtual bool ChalengeResponse(const CString& AuthHeader, CString * AuthResponse);    
};

#endif

