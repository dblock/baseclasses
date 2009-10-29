/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________
    
    written by Daniel Doubrovkine - dblock@vestris.com
  
    Things to do:
    
        POST method, www-url-encoded and multipart        
      
    Revision history:
        
        06.08.1999: sending \10\10 after HTTP requests (hack for ala WebStar servers)
        18.03.2000: escape URL path in MakeRequestString (file names containing spaces)
        06.06.2000: ISM/NT4 returns 406 for a file 1.0 GET when file exists
          
*/

#include <baseclasses.hpp>

#include "HttpRequest.hpp"
#include <Encryption/Encryption.hpp>
#include "HtmlParser.hpp"
#include "MimeParser.hpp"
#include <String/GStrings.hpp>

CHttpRequest::CHttpRequest(void) {
    Trace(tagHttp, levInfo, ("CHttpRequest - CHttpRequest"));
    m_RequestMethod = htGet;
    m_RequestSizeLimit = -1;
    m_FollowRedirections = true;
    m_RStatusValue = -1;
    m_ServerAuthState.SetResponseHeaders(& m_RPairFields);
    m_Dump = false;
}

CHttpRequest::~CHttpRequest(void) {
    
}

void CHttpRequest::ClearResults(bool Redirections) {
    Trace(tagHttp, levInfo, ("CHttpRequest - ClearResults"));
    m_RFields.RemoveAll();
    m_RPairFields.RemoveAll();
    if (Redirections) 
      m_RRedirections.RemoveAll();
    m_RStatusValue = -1;
    m_RStatusString.Empty();
    m_RData.Empty();
}

int CHttpRequest::ExecuteGet(bool Recurse) {

    bool bContinueExecuteGet = false;
    bool bKeepAlive = false;

    Trace(tagHttp, levInfo, ("CHttpRequest - ExecuteGet"));

    unsigned int nCredentialsIndex = 0;

    bool Result;

    do {

        if (! bKeepAlive) {
            Result = CreateSocket();
            
            if (! Result) {
                return -1;
            }
        }

        bContinueExecuteGet = false;

        // process authentication

        CString AuthHeader;
        
        switch(m_ServerAuthState.GetLeg()) {
        case AUTHENTICATION_STATE_NONE:            
            break;
        case AUTHENTICATION_STATE_PRECHALENGE:            
        case AUTHENTICATION_STATE_CHALENGE:
            if (! m_ServerAuthState.GetNextHeader(& AuthHeader, nCredentialsIndex))
                return m_RStatusValue;
            break;
        }

        ClearResults(false);

        // add the auth header
        if (AuthHeader.GetLength())
            SetHttpField(g_strHttpAuthorization, AuthHeader);

        /* create socket and get results */
                
        CStringTable Connection;
        if (!m_HttpFields.FindAndCopy(g_strHttpConnection, Connection) || 
            ! Connection.GetValue(g_strHttpConnection).GetLength()) {            
            SetHttpField(g_strHttpConnection, "Keep-Alive");
        }       
        
        
        Result = GetHTTP((float) 1.0);

        // 406: no acceptable objects found (NT4/ISM)
        // if (!Result || (m_RStatusValue == 406)) {            
        //    ClearResults(false);
        //    Result = CreateSocket() && GetHTTP((float) 0.9);
        //    if (! Result) {
        //        return -1;
        //    }
        // }

        Trace(tagHttp, levInfo, ("CHttpRequest - ExecuteGet {%d/%d}", Result, m_RStatusValue));        
        
        if (! Result) {
            return -1;
        }
        
        CString l_RedirectLoc;
        CUrl ResolvedUrl;

        switch(m_RStatusValue) {
        case 407:
            // $(TODO)
            break;
            
        case 401:        

            // any valid authentication stage
            if (m_ServerAuthState.GetLeg() == AUTHENTICATION_STATE_NONE) {         
                // we need to send credentials
                m_ServerAuthState.SetLeg(AUTHENTICATION_STATE_PRECHALENGE);
                bContinueExecuteGet = true;                
            } else if (m_ServerAuthState.GetLeg() == AUTHENTICATION_STATE_CHALENGE) {
                nCredentialsIndex ++;
                // equal, because we do want to run once without username/password credentials
                if (nCredentialsIndex <= m_ServerAuthState.GetSize()) {
                    bContinueExecuteGet = true;
                }
                m_ServerAuthState.SetLeg(AUTHENTICATION_STATE_PRECHALENGE);
                bKeepAlive = false;
            } else if (m_ServerAuthState.GetLeg() == AUTHENTICATION_STATE_PRECHALENGE) {
                m_ServerAuthState.SetLeg(AUTHENTICATION_STATE_CHALENGE);
                bContinueExecuteGet = true;
                bKeepAlive = true;
            }
            
            break;
            
        case 301: /* redirections */
        case 302:
        case 303:
        case 307:

            l_RedirectLoc = m_RFields.FindElement(g_strHttpLocation).GetValue(g_strHttpLocation);
            
            if (! l_RedirectLoc.GetLength()) {
                Trace(tagHttp, levInfo, ("CHttpRequest - GetHTTP - %d redirection without a Location: header.", m_RStatusValue));		      
                break;
	    }
                
            // HTTP 1.1 - Temporary Redirect is 302 and 307
            // m_RRedirections is relevant for final URL address
            // that could be retrieved
            
            ResolvedUrl = m_Url.Resolve(l_RedirectLoc);        
            l_RedirectLoc = ResolvedUrl.GetHttpAll();        
            
            if (m_FollowRedirections && m_RRedirections.Contains(l_RedirectLoc)) {
                // avoid circular redirections
                Trace(tagHttp, levInfo, ("CHttpRequest - GetHTTP - circular redirection %s", l_RedirectLoc.GetBuffer()));		      
                return m_RStatusValue;
            } else m_RRedirections.Add(l_RedirectLoc);
            
            if (!m_FollowRedirections)
                return m_RStatusValue;                
            
            if (m_ClientSocket.GetVerbose()) 
                cout << "\n\t[" << l_RedirectLoc << "]";
            
            ClearResults(false);
            m_Url.SetUrl(l_RedirectLoc);
            bContinueExecuteGet = true;
            
            break;
            
        case 305: /* use proxy */
            
            l_RedirectLoc = m_RFields.FindElement(g_strHttpLocation).GetValue(g_strHttpLocation);
            
            if (! l_RedirectLoc.GetLength())
                break;

            // HTTP 1.1 - Temporary Redirect is 302 and 307
            // m_RRedirections is relevant for final URL address
            // that could be retrieved
            
            ClearResults(false);
            m_Proxy.SetUrl(l_RedirectLoc);

            bContinueExecuteGet = true;

            break;
        };

        Trace(tagHttp, levInfo, ("CHttpRequest - GetHTTP - %d", m_RStatusValue));        

    } while (bContinueExecuteGet);

    return m_RStatusValue;
}

bool CHttpRequest::Execute(const CUrl& Url) {
    Trace(tagHttp, levInfo, ("CHttpRequest - CHttpRequest {%s}", Url.GetBrute().GetBuffer()));
    m_Url = Url;
    ClearResults(true);
    switch(m_RequestMethod) {
    case htPost:
        assert(0);
        if (CreateSocket()) {
        }
        break;
    case htGet:
        if (m_RequestSizeLimit == 0) m_RequestMethod = htHead;
    case htHead:
        return (ExecuteGet(true) != -1);
    default:
        return false;
    }
    return false;
}

bool CHttpRequest::CreateSocket(void) {
    Trace(tagHttp, levInfo, ("CHttpRequest - CreateSocket"));
    CString Server;
    int Port;
    if (m_Proxy.GetValid()) {
        Server = m_Proxy.GetHost();
        Port = m_Proxy.GetPortValue();
    } else if (m_Url.GetValid()) {
        Server = m_Url.GetHost();
        Port = m_Url.GetPortValue();
    } else {
        m_RStatusValue = 400; // bad request
        return false;
    }
    Trace(tagHttp, levInfo, ("CHttpRequest - CreateSocket {%s}", Server.GetBuffer()));
    m_ClientSocket.SetSocket(Port, Server);
    
    bool bResult = m_ClientSocket.Open();

    if (! bResult) {        
        m_RStatusValue = m_ClientSocket.GetRStatus();
        m_RStatusString = m_ClientSocket.GetError();
        return false;
    }

    m_ClientSocket.SetSendTcpWindowSize();
    m_ClientSocket.SetRecvTcpWindowSize();

    // if (bResult) bResult = m_ClientSocket.Bind();
    bResult = m_ClientSocket.Connect();

    if (! bResult) {        
        m_RStatusValue = m_ClientSocket.GetRStatus();
        m_RStatusString = m_ClientSocket.GetError();
        return false;
    }
    
    return true;
}

void CHttpRequest::MakeRequestString(const float HttpVersion, CString * pRequest) {
    
    pRequest->SetSize(128 + m_HttpFields.GetSize() * 32);
    (* pRequest) = CHttpIo::HTTPIoTypeToStr(m_RequestMethod) + ' ';
    CString HostString = m_Url.GetHost();
    int DefaultPortValue = 80; 
    if (m_Url.GetScheme().Same(g_strProto_HTTPS)) DefaultPortValue = 443;
    if (m_Url.GetPortValue() != DefaultPortValue) {
      HostString.Append(':');
      HostString.Append(m_Url.GetPort());
    }
    if (m_Proxy.GetValid() || (HttpVersion < 1)) {      
      pRequest->Append(m_Url.GetScheme());
      pRequest->Append(g_strColSlashSlash);
      pRequest->Append(HostString);
    }
    CString UrlPath = m_Url.GetUrlPath();
    CHtmlParser::TranslateQuotes(UrlPath);
    pRequest->Append(CUrl::Escape(UrlPath, false));
    if (HttpVersion >= 1.0) {
      pRequest->Append(g_strHttpSpaceOneZero);
      CStringTable Table;
      Table.Set(g_strName, g_strHttpHost);
      Table.Set(g_strValue, HostString);
      m_HttpFields.Set(g_strHttpHost, Table);
    } else m_HttpFields.Remove(g_strHttpHost);
    pRequest->Append(g_strCrLf);
    for (register int i=0;i<(int) m_HttpFields.GetSize();i++) {
      pRequest->Append(m_HttpFields.GetKeyAt(i));
      pRequest->Append(g_strColSpace);
      pRequest->Append(m_HttpFields[i].GetValue(g_strValue));
      for (register int j=0;j<(int)m_HttpFields[i].GetSize();j++) {
        if ((!m_HttpFields[i].GetNameAt(j).Same(g_strName)) &&
            (!m_HttpFields[i].GetNameAt(j).Same(g_strValue))) {
          pRequest->Append(g_strSemiColSpace);
          pRequest->Append(m_HttpFields[i].GetNameAt(j));
          pRequest->Append('=');
          pRequest->Append(m_HttpFields[i].GetValueAt(j));
        }
      }
      pRequest->Append(g_strCrLf);
    }    
    pRequest->Append(g_strCrLf);
}

bool CHttpRequest::ParseStatusString(void) {
    
    static const CString __HttpSlash("HTTP/");

    if (! m_RStatusString.GetLength())
        return false;
        
    int sPos = m_RStatusString.Pos(' ');
        
    if (sPos < 0)
        return false;
            
    m_RStatusString.Left(sPos, &m_RStatusVersion);
            
    if (! m_RStatusVersion.StartsWith(__HttpSlash))
        return false;
    
    CString MidString;
                
    m_RStatusVersion.Mid(__HttpSlash.GetLength(), m_RStatusVersion.GetLength(), &MidString);
    Trace(tagHttp, levInfo, ("CHttpRequest - StatusVersion {%s}", MidString.GetBuffer()));
                
    m_RStatusVersionValue = CString::StrToFloat(MidString);                
    Trace(tagHttp, levInfo, ("CHttpRequest - StatusVersion {%s:%f}", m_RStatusVersion.GetBuffer(), m_RStatusVersionValue));
    
    // get the value from HTTP/M.M XXX String
    int vPos = m_RStatusString.Pos(' ', sPos + 1);
    if (vPos > sPos) {
        m_RStatusValue = m_RStatusString.GetInt(sPos + 1, vPos - sPos - 1);
    } else {
        // get the case without a description HTTP/M.M XXX
        if (! m_RStatusString.IsInt(sPos + 1, m_RStatusString.GetLength(), & m_RStatusValue))
            return false;
    }

    Trace(tagHttp, levInfo, ("CHttpRequest - StatusVersion {%d}", m_RStatusValue));

    return true;
}

bool CHttpRequest::GetHTTP(float HTTPVersion) {   
    
    /* HTTP request */
    CString Request;
    MakeRequestString(HTTPVersion, &Request);

    Trace(tagHttp, levInfo, ("CHttpRequest - Request {\n%s}", Request.GetBuffer()));
    
    if (m_Dump) {
        cout << "-=-=-=-=-=-=-=-=- Request -=-=-=-=-=-=-=-=-" << endl 
             << Request
             << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    }
    
    if (!m_ClientSocket.Write(Request))
        return false;
    
    /* HTTP response, header */
    m_ClientSocket.ReadLine(&m_RStatusString);

    Trace(tagHttp, levInfo, ("CHttpRequest - StatusString {%s}", m_RStatusString.GetBuffer()));

    if (! ParseStatusString()) {
        m_RData = m_RStatusString;
    }

    if (m_RStatusValue > 0) {
        // finish reading headers
        CString OneLine;
        m_ClientSocket.ReadLine(&OneLine);

        if (m_Dump) {
            cout << "-=-=-=-=-=-=-=-=- Response -=-=-=-=-=-=-=-" << endl 
                 << m_RStatusString
                 << endl;
        }
        
        Trace(tagHttp, levInfo, ("CHttpRequest - Response (start)    : %s", m_RStatusString.GetBuffer()));
        
        while (OneLine.GetLength()) {
    
            if (m_Dump) {
                cout << OneLine << endl;
            }
            
            CStringTable Table;
            CString Name, Value;            
            if (CMimeParser::ParseLine(OneLine, Name, Value, Table)) {
                Trace(tagHttp, levInfo, ("CHttpRequest - Response (continued): %d | %s <table> (%d)", OneLine.GetLength(), Name.GetBuffer(), Table.GetSize()));
                Trace(tagHttp, levInfo, ("CHttpRequest -                       %s", OneLine.GetBuffer()));
                m_RFields.Add(Name, Table);
                m_RPairFields.Add(CStringPair(Name, Value));
            } else {
                Trace(tagHttp, levInfo, ("CHttpRequest - Response (continued): %s (parser failed, ignoring)", OneLine.GetBuffer()));
                // ignore malformed lines
                // $(TODO): support P3P, which looks like P3P CP= ...
                // m_RStatusValue = 400
                // return false;                
            }
            m_ClientSocket.ReadLine(&OneLine);
        }
    
        if (m_Dump) {
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
        }
        
        Trace(tagHttp, levInfo, ("CHttpRequest - Response (done)"));
    }
    
    if ((m_RequestMethod != htGet)&&(m_RequestMethod != htPost)) 
        return true;

    CString ContentLengthString = m_RFields.FindElement(g_strHttpContentLength).GetValue(g_strHttpContentLength);
    
    int ContentLengthExpected = 0;
    
    if (! ContentLengthString.IsInt(& ContentLengthExpected))
        ContentLengthExpected = -1;
    
    Trace(tagHttp, levInfo, ("CHttpRequest::GetHTTP() - expected content-length: %s {%d}", ContentLengthString.GetBuffer(), ContentLengthExpected));        

    m_RData.Empty();
    
    // document is going to be larger than the current maximum document size limit
    if ((ContentLengthExpected > 0) && (m_RequestSizeLimit > 0) && (m_RequestSizeLimit < ContentLengthExpected)) {
        
        m_RStatusValue = 206; // partial content        
        
    } else {
        
        if (ContentLengthExpected > 0) {
    
            m_ClientSocket.Read(& m_RData, ContentLengthExpected);
            
            if (ContentLengthExpected != (int) m_RData.GetLength()) {
	      Trace(tagHttp, levInfo, ("CHttpRequest::GetHTTP() - expected %d bytes, read %d. (207 Partial Content)", ContentLengthExpected, m_RData.GetLength()));        
	      m_RStatusValue = 207; // partial Content-Length
	      return false;
            }
            
        } else if ((ContentLengthExpected < 0) && (m_RStatusValue == 200)) {
            // content-length was not explicitly zero and the status value says that data should be available
            m_ClientSocket.Read(& m_RData, m_RequestSizeLimit);        
        }
        
        if (m_RStatusValue <= 0) {
            m_RStatusValue = (m_RData.GetLength() > 0) ? 200 : 204;
            
            if (!m_RData.GetLength())
                return false;
        }
    }
    
    Trace(tagHttp, levInfo, ("CHttpRequest::GetHTTP() - m_RStatusValue {%d}", m_RStatusValue));        
    Trace(tagHttp, levInfo, ("CHttpRequest::GetHTTP() - m_RData.Length {%d}", m_RData.GetLength()));        
    
    return true;
}

void CHttpRequest::SetHttpField(const CString& Name, const CString& Value) {
    CStringTable Table = m_HttpFields.FindElement(Name);
    Table.Set(g_strName, Name);
    Table.Set(g_strValue, Value);
    m_HttpFields.Set(Name, Table);
}

void CHttpRequest::SetHttpField(const CString& Group, const CString& Name, const CString& Value) {
    CStringTable Table = m_HttpFields.FindElement(Group);
    Table.Set(Name, Value);
    m_HttpFields.Set(Group, Table);
}

void CHttpRequest::SetRField(const CString& Name, const CString& Value) {
    CStringTable Table = m_RFields.FindElement(Name);
    // Table.Set(g_strName, Name);
    // Table.Set(g_strValue, Value);
    Table.Set(Name, Value);
    m_RFields.Set(Name, Table);
}

void CHttpRequest::SetRField(const CString& Group, const CString& Name, const CString& Value) {
    CStringTable Table = m_RFields.FindElement(Group);
    Table.Set(Name, Value);
    m_RFields.Set(Group, Table);
}

void CHttpRequest::SetPostField(const CString& Name, const CString& Value) {
    CStringTable Table = m_PostFields.FindElement(Name);
    Table.Set(g_strName, Name);
    Table.Set(g_strValue, Value);
    m_PostFields.Set(Name, Table);
}

void CHttpRequest::SetPostField(const CString& Group, const CString& Name, const CString& Value) {
    CStringTable Table = m_PostFields.FindElement(Group);
    Table.Set(Name, Value);
    m_PostFields.Set(Group, Table);
}
