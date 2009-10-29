/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#ifdef _WIN32

#include "SspiAuth.hpp"

CWin32SecInterface CSspiAuthenticator :: m_SecInterface;

CSspiAuthenticator :: CSspiAuthenticator(CAuthenticationType AuthType, const CString& Package) :
    CAuthenticator(AuthType) {

    m_Package = Package;
    Initialize();
}

CSspiAuthenticator :: ~CSspiAuthenticator(void) {
    UnInitialize();
}

void CSspiAuthenticator :: Initialize(void) {           
    CWin32SecInterface :: SetInvalidCredHandle(& m_Credentials);    
    CWin32SecInterface :: SetInvalidCtxtHandle(& m_Context);
    m_HaveCredentials = false;
    m_HaveContext = false;    
    m_MaxTokenSize = 0;
    QueryMaxToken();
}

void CSspiAuthenticator :: UnInitialize(void) {
        
    if (! CWin32SecInterface :: IsInvalidCredHandle(m_Credentials))
	{		
        m_SecInterface.GetSfProcs().pFreeCredentialsHandle(& m_Credentials);
        CWin32SecInterface :: SetInvalidCredHandle(&m_Credentials);		
	}

    if(! CWin32SecInterface :: IsInvalidCtxtHandle(m_Context)) {				
        m_SecInterface.GetSfProcs().pDeleteSecurityContext(& m_Context);
		CWin32SecInterface :: SetInvalidCtxtHandle(& m_Context);		
	}
}

bool CSspiAuthenticator :: AcquireCredentials(void) {
    
    SECURITY_STATUS           Status;
    SEC_WINNT_AUTH_IDENTITY   AuthIdentity;
    SEC_WINNT_AUTH_IDENTITY * pAuthIdentity = NULL;
    TimeStamp                 Lifetime;
    
    if (m_Username.GetLength() || m_Password.GetLength()) {
        pAuthIdentity = &AuthIdentity;

        memset(pAuthIdentity, 0, sizeof(SEC_WINNT_AUTH_IDENTITY));
        
        if (m_Username.GetLength()) {
            AuthIdentity.User = (unsigned char *) m_Username.GetBuffer();
            AuthIdentity.UserLength = (ULONG) m_Username.GetLength();
        }

        if (m_Password.GetLength()) {
            AuthIdentity.Password = (unsigned char *) m_Password.GetBuffer();
            AuthIdentity.PasswordLength = (ULONG) m_Password.GetLength();
        }

        if (m_Domain.GetLength()) {
            AuthIdentity.Domain = (unsigned char *) m_Domain.GetBuffer();
            AuthIdentity.DomainLength = (ULONG) m_Domain.GetLength();
        }

        AuthIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
    }

    Status = m_SecInterface.GetSfProcs().pAcquireCredentialsHandle(
        NULL,             // New principal
        (char *) m_Package.GetBuffer(), // Package name
        SECPKG_CRED_OUTBOUND,
        NULL,             // Logon ID
        pAuthIdentity,    // Auth Data
        NULL,             // Get key func
        NULL,             // Get key arg
        & m_Credentials,
        & Lifetime );

    if (Status < 0)
        return false;

    m_HaveCredentials = true;

    return true;
}

bool CSspiAuthenticator :: QueryMaxToken(void) {

    SECURITY_STATUS Status;
    SecPkgInfo * pspkg;

    Status = m_SecInterface.GetSfProcs().pQuerySecurityPackageInfo(
        (char *) m_Package.GetBuffer(),
        & pspkg );

    if ( Status != 0 )
        return false;
    
    m_MaxTokenSize = pspkg->cbMaxToken;
    
    m_SecInterface.GetSfProcs().pFreeContextBuffer(pspkg);
    return true;
}

bool CSspiAuthenticator :: Chalenge(CString * AuthResponse) {
    CString Dummy;

    return Context(Dummy, AuthResponse);
}

bool CSspiAuthenticator :: Response(const CString& AuthHeader, CString * AuthResponse) {    
    
    CString AuthDecoded(AuthHeader);

    if (! AuthDecoded.StartsWithSame(m_Package))
        return false;

    AuthDecoded.Delete(0, m_Package.GetLength() + 1);

    AuthDecoded = CEncryption::Base64Decode(AuthDecoded);

    return Context(AuthDecoded, AuthResponse);
}

bool CSspiAuthenticator :: Context(const CString& ServerBuffer, CString * AuthResponse) {

    CString Buffer;

    SecBufferDesc         OutBuffDesc;
    SecBuffer             OutSecBuff;
    SecBufferDesc         InBuffDesc;
    SecBuffer             InSecBuff;
    SECURITY_STATUS       Status;
    ULONG                 ContextAttributes;
    TimeStamp             Lifetime;
    BOOL                  fReply;

    Buffer.SetSize(m_MaxTokenSize);

    OutBuffDesc.ulVersion = 0;
    OutBuffDesc.cBuffers  = 1;
    OutBuffDesc.pBuffers  = & OutSecBuff;

    OutSecBuff.cbBuffer   = m_MaxTokenSize;
    OutSecBuff.BufferType = SECBUFFER_TOKEN;
    OutSecBuff.pvBuffer   = (LPVOID) Buffer.GetBuffer();

    if (ServerBuffer.GetLength()) {

        InBuffDesc.ulVersion = 0;
        InBuffDesc.cBuffers  = 1;
        InBuffDesc.pBuffers  = & InSecBuff;

        InSecBuff.cbBuffer   = ServerBuffer.GetLength() + 1;
        InSecBuff.BufferType = SECBUFFER_TOKEN;
        InSecBuff.pvBuffer   = (LPVOID) ServerBuffer.GetBuffer();
    }


    Status = m_SecInterface.GetSfProcs().pInitializeSecurityContext(
        & m_Credentials,
        ServerBuffer.GetLength() ? & m_Context : NULL,
        "InetSvcs",
        0,
        0,
        SECURITY_NATIVE_DREP,
        ServerBuffer.GetLength() ? & InBuffDesc : NULL,
        0,
        & m_Context,
        & OutBuffDesc,
        & ContextAttributes,
        & Lifetime);

    if (Status < 0)
        return false;
        
    fReply = !! OutSecBuff.cbBuffer;

    if ((Status == SEC_I_COMPLETE_NEEDED) || (Status == SEC_I_COMPLETE_AND_CONTINUE)) {

        if ( m_SecInterface.GetSfProcs().pCompleteAuthToken == NULL )
            return false;

        Status = m_SecInterface.GetSfProcs().pCompleteAuthToken( 
            & m_Context,
            & OutBuffDesc );
        
        if ( Status < 0)
        {
            return false;
        }        
    }

    if (! fReply)
        return true;

    CString OutputBuffer;

    OutputBuffer.CopyBuffer(
        (const char *) OutSecBuff.pvBuffer, 
        OutSecBuff.cbBuffer);

    (* AuthResponse) = m_Package;
    (* AuthResponse) += ' ';
    (* AuthResponse) += CEncryption::Base64Encode(OutputBuffer);

    return true;
}

bool CSspiAuthenticator :: ChalengeResponse(const CString& AuthHeader, CString * AuthResponse) {

    if(AuthHeader.Same(m_Package)) {
        
        if (! AcquireCredentials())
            return false;

        return Chalenge(AuthResponse);
    }

    return Response(AuthHeader, AuthResponse);    
}

#endif
