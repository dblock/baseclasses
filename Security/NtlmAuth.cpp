/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "NtlmAuth.hpp"
#include "Win32SecInterface.hpp"

#ifdef _WIN32

CNtlmAuthenticator :: CNtlmAuthenticator(void) :
    CSspiAuthenticator(AUTHENTICATION_TYPE_NTLM, "NTLM") {

    
}

CNtlmAuthenticator :: ~CNtlmAuthenticator(void) {

    
}

CNegotiateAuthenticator :: CNegotiateAuthenticator(void) :
    CSspiAuthenticator(AUTHENTICATION_TYPE_NEGOTIATE, "NEGOTIATE") {

    
}

CNegotiateAuthenticator :: ~CNegotiateAuthenticator(void) {

    
}

#endif
