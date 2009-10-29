/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "BasicAuth.hpp"
#include <Encryption/Encryption.hpp>

CBasicAuthenticator :: CBasicAuthenticator(void) :
    CAuthenticator(AUTHENTICATION_TYPE_BASIC) {

}

CBasicAuthenticator :: ~CBasicAuthenticator(void) {

}
 
bool CBasicAuthenticator :: ChalengeResponse(const CString& AuthHeader, CString * AuthResponse) {

    // the WWW-Authenthicate header returns: WWW-Authenticate: Basic realm="Name"
    // response must be:
    // basic-credentials = "Basic" SP basic-cookie
    // basic-cookie      = <base64 encoding of userid-password,
    // except not limited to 76 char/line>
    // userid-password   = [ token ] ":" *TEXT
    // ex: (Alladin-open sesame) => Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==

    if (! AuthResponse)
        return false;

    if (! m_Username.GetLength())
        return false;

    if (! AuthHeader.StartsWithSame("BASIC "))
        return false;

    CString UsernamePass;
    
    UsernamePass += m_Username;
    UsernamePass += ':';
    UsernamePass += m_Password;

    (* AuthResponse) = "Basic ";
    (* AuthResponse) += CEncryption::Base64Encode(UsernamePass);

    return true;
}
    


