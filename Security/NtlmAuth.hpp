/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_NTLM_AUTHENTICATION_HPP
#define BASECLASSES_NTLM_AUTHENTICATION_HPP

#include <platform/include.hpp>

#ifdef _WIN32

#include "Authentication.hpp"
#include "SspiAuth.hpp"

class CNtlmAuthenticator : public CSspiAuthenticator {
public:
    CNtlmAuthenticator(void);
    virtual ~CNtlmAuthenticator(void);
};

class CNegotiateAuthenticator : public CSspiAuthenticator {
public:
    CNegotiateAuthenticator(void);
    virtual ~CNegotiateAuthenticator(void);
};

#endif

#endif
