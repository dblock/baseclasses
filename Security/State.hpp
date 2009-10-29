/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_AUTHENTICATION_STATE_HPP
#define BASECLASSES_AUTHENTICATION_STATE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <String/GStrings.hpp>
#include <String/StringTable.hpp>
#include <HashTable/VectorTable.hpp>

typedef enum {
    AUTHENTICATION_STATE_NONE,
    AUTHENTICATION_STATE_PRECHALENGE,
    AUTHENTICATION_STATE_CHALENGE
} CAuthenticationLeg;

typedef enum {
    AUTHENTICATION_TYPE_NONE = 0x1,
    AUTHENTICATION_TYPE_BASIC = 0x2,
    AUTHENTICATION_TYPE_NTLM = 0x4,
    AUTHENTICATION_TYPE_NEGOTIATE = 0x8,
} CAuthenticationType;

class CAuthenticator;

class CAuthenticationState {
    copy_property(CAuthenticator *, Authenticator);
    copy_property(CVector<CStringPair> *, ResponseHeaders);
    property(unsigned int, AvailableTypes);
    property(CAuthenticationType, Type);
    property(CAuthenticationLeg, Leg);
    property(CVector<CStringPair>, Auth);
    property(int, SucceededAuthIndex);
public:
    CAuthenticationState(void);
    ~CAuthenticationState(void);
    bool GetNextHeader(CString * pAuthenticationString, unsigned int nCredentialsIndex);
    bool LoadAuthenticator(CAuthenticationType AuthType, unsigned int nCredentialsIndex);
    static CAuthenticationType GetAuthType(const CString& AuthString);
    bool GetCredentialsAt(unsigned int nIndex, CString * Username, CString * Password, CString * Domain) const;
    inline unsigned int GetSize(void) const { return m_Auth.GetSize(); }
    inline void AddAuth(const CStringPair& Credentials) { m_Auth += Credentials; }
    inline void AddAuth(const CString& Username, const CString& Password) { AddAuth(CStringPair(Username, Password)); }
};


#endif
