/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "State.hpp"
#include "BasicAuth.hpp"
#include "NtlmAuth.hpp"

CAuthenticationState :: CAuthenticationState(void) :
    m_Authenticator(NULL),
    m_ResponseHeaders(NULL),
    m_AvailableTypes(
        AUTHENTICATION_TYPE_BASIC 
#ifdef _WIN32
        | AUTHENTICATION_TYPE_NTLM
        // | AUTHENTICATION_TYPE_NEGOTIATE
#endif
        ),
    m_Type(AUTHENTICATION_TYPE_NONE),
    m_Leg(AUTHENTICATION_STATE_NONE),
    m_SucceededAuthIndex(-1) {

}

CAuthenticationState :: ~CAuthenticationState(void) {
    if (m_Authenticator != NULL)
        delete m_Authenticator;
    
}

bool CAuthenticationState :: GetNextHeader(CString * pAuthenticationString, unsigned int nCredentialsIndex) {

    bool bResult = false;

    if (! m_ResponseHeaders)
        return false;

    for (int i = 0; i < (int) m_ResponseHeaders->GetSize(); i++) {
        if ((* m_ResponseHeaders)[i].GetName().Same(g_strHttpWWWAuthenticate)) {            
            
            CAuthenticationType AuthType = GetAuthType((* m_ResponseHeaders)[i].GetValue());

            if ((m_AvailableTypes & AuthType) == 0)
                continue;

            if (LoadAuthenticator(AuthType, nCredentialsIndex)) {
                
                // basic auth does not have a pre-chalenge step, reset
                if ((AuthType == AUTHENTICATION_TYPE_BASIC) && (m_Leg == AUTHENTICATION_STATE_PRECHALENGE))
                    m_Leg = AUTHENTICATION_STATE_CHALENGE;

                if (m_Authenticator->ChalengeResponse((* m_ResponseHeaders)[i].GetValue(), pAuthenticationString)) {
                    bResult = true;
                    m_SucceededAuthIndex = nCredentialsIndex;
                    break;
                }
            }
        }
    }

    return bResult;
}

bool CAuthenticationState :: GetCredentialsAt(unsigned int nIndex, CString * Username, CString * Password, CString * Domain) const {
    
    if (nIndex >= m_Auth.GetSize())
        return false;

    if (Username)
        * Username = m_Auth[nIndex].GetName();
    if (Password)
        * Password = m_Auth[nIndex].GetValue();

    int nPos = Username ? Username->Pos('\\') : -1;

    if (nPos >= 0) {
        if (Domain && Username) 
            Username->Mid(0, nPos, Domain);
        if (Username)
            Username->Delete(0, nPos + 1);
    } else {        
        if (Domain)
            Domain->Empty();
    }

    return true;
}

bool CAuthenticationState :: LoadAuthenticator(CAuthenticationType AuthType, unsigned int nCredentialsIndex) {

    bool bResult = true;
    bool bCreateAuthenticator = true;

    if (m_Authenticator != NULL) {
        if (m_Authenticator->GetAuthType() == AuthType) {
            bCreateAuthenticator = false;
        } else {
            delete m_Authenticator;
            m_Authenticator = NULL;
        }
    }

    if (bCreateAuthenticator) {
        switch(AuthType) {
        case AUTHENTICATION_TYPE_BASIC:
            m_Authenticator = new CBasicAuthenticator;
            break;
#ifdef _WIN32
        case AUTHENTICATION_TYPE_NTLM:
            m_Authenticator = new CNtlmAuthenticator;
            break;
        case AUTHENTICATION_TYPE_NEGOTIATE:
            m_Authenticator = new CNegotiateAuthenticator;
            break;
#endif
        default:
            return false;
        }
    }

    CString Username;
    CString Domain;
    CString Password;

    if (GetCredentialsAt(nCredentialsIndex, & Username, & Password, & Domain)) {
        m_Authenticator->SetUsername(Username);
        m_Authenticator->SetPassword(Password);
        m_Authenticator->SetDomain(Domain);
    }

    return bResult;
}

CAuthenticationType CAuthenticationState :: GetAuthType(const CString& AuthString) {    
    if (AuthString.StartsWithSame("BASIC")) {
        return AUTHENTICATION_TYPE_BASIC;
    }
#ifdef _WIN32
    else if (AuthString.StartsWithSame("NTLM")) {        
        return AUTHENTICATION_TYPE_NTLM;
    }
    else if (AuthString.StartsWithSame("NEGOTIATE")) {        
        return AUTHENTICATION_TYPE_NEGOTIATE;
    }
#endif
    return AUTHENTICATION_TYPE_NONE;
}

