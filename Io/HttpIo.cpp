/*

    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com
  
    Revision history:

        04.08.1999: automatic support for CGI (when m_Connection is NULL)
        05.08.1999: fixed stdin under NT reopened in binary mode in ReadClient
        13.09.1999: moved Mime parsers to CMimeParser
        13.09.1999: moved multipart parser to CMimeParser
        10.06.2000: flushing buffer at once
    
*/

#include <baseclasses.hpp>

#include "HttpIo.hpp"
#include <Internet/MimeParser.hpp>
#include <Server/Connection.hpp>
#include <String/GStrings.hpp>
#include <File/MMapFile.hpp>
#include <File/LocalFile.hpp>
#include <Encryption/Encryption.hpp>
#include <Date/Date.hpp>

CAtomic CHttpIo::m_TotalTraffic;

static const CHttpIoFileTypes g_HttpIoFileTypes[] = {
    { ".html",      "text/html",                hftDefault | hftDocument },
    { ".wml",       "text/vnd.wap.wml",         hftDefault | hftDocument },
    { ".htm",       "text/html",                hftDefault | hftDocument },
    { ".js",        "application/x-javascript", hftScript },
    { ".css",       "text/stylesheet",          hftStylesheet },
    { ".gif",       "image/gif",                hftImage },
    { ".tif",       "image/tiff",               hftImage },
    { ".txt",       "text/plain",               hftDocument },
    { ".jpg",       "image/jpeg",               hftImage },
    { ".jpeg",      "image/jpeg",               hftImage },
    { ".rtx",       "text/richtext",            hftDocument },
    { ".ief",       "image/ief",                hftImage },
    { ".jpe",       "image/jpeg",               hftImage },
    { ".png",       "image/png",                hftImage },
    { ".tiff",      "image/tiff",               hftImage },
    { ".doc",       "application/msword",       hftDocument },
    { ".bin",       "application/octet-stream", hftDocument },
    { ".lha",       "application/octet-stream", hftDocument },
    { ".lzh",       "application/octet-stream", hftDocument },
    { ".exe",       "application/octet-stream", hftDocument },
    { ".dms",       "application/octet-stream", hftDocument },
    { ".pdf",       "application/pdf",          hftDocument },
    { ".ai",        "application/postscript",   hftDocument },
    { ".eps",       "application/postscript",   hftDocument },
    { ".ps",        "application/postscript",   hftDocument },
    { ".ppt",       "application/powerpoint",   hftDocument },
    { ".rtf",       "application/rtf",          hftDocument },
    { ".dvi",       "application/x-dvi",        hftDocument },
    { ".tar",       "application/x-tar",        hftDocument },
    { ".tex",       "application/tex",          hftDocument },
    { ".zip",       "application/zip",          hftDocument },
    { ".ras",       "image/x-cmu-raster",       hftImage },
    { ".pnm",       "image/x-portable-anymap",  hftImage },
    { ".pbm",       "image/x-portable-bitmap",  hftImage },
    { ".pgm",       "image/x-portable-graymap", hftImage },
    { ".ppm",       "image/x-portable-pixmap",  hftImage },
    { ".rgb",       "image/x-rgb",              hftImage },
    { ".xbm",       "image/x-xbitmap",          hftImage },
    { ".xpm",       "image/x-bpixmap",          hftImage },
    { ".sgml",      "text/sgml",                hftDocument },
    { ".sgm",       "text/sgml",                hftDocument },
};

CHttpIo::CHttpIo(void) : CIo() {
    m_Connection = NULL;
    Initialize();
}

CHttpIo::CHttpIo(const CConnection& Connection) : CIo() {
    m_Connection = & (CConnection&) Connection;
    Initialize();
}

CString CHttpIo::HTTPIoTypeToStr(const CHttpIoType& Type) {
    switch(Type) {
    case htGet: return g_strHttpGet;
    case htPost: return g_strHttpPost;
    case htOptions: return g_strHttpOptions;
    case htHead: return g_strHttpHead;
    case htPut: return g_strHttpPut;
    case htDelete: return g_strHttpDelete;
    case htTrace: return g_strHttpTrace;
    case htConnect: return g_strHttpConnect;
    default: break;
    }
    return CString::EmptyCString;
}

CHttpIoType CHttpIo::StrToHTTPIoType(const CString& String) {
    if (String.Same(g_strHttpGet)) return htGet;
    else if (String.Same(g_strHttpPost)) return htPost;
    else if (String.Same(g_strHttpOptions)) return htOptions;
    else if (String.Same(g_strHttpHead)) return htHead;
    else if (String.Same(g_strHttpPut)) return htPut;
    else if (String.Same(g_strHttpDelete)) return htDelete;
    else if (String.Same(g_strHttpTrace)) return htTrace;
    else if (String.Same(g_strHttpConnect)) return htConnect;
    else return htUnknown;
}

bool CHttpIo::ProcessMethod(void) {
    if (m_Connection) {
        
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - server request."));
        
        /* server request */
        CString CommandLine;
    
        // reset input request type
        m_InputType = htUnknown;
        
        if (! m_Connection->ReadLine(&CommandLine)) {
            Trace(tagHttp, levError, ("CHttpIo::ProcessMethod - server request - no query line, aborted."));
            return false;
        }
        
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - read [%s].", CommandLine.GetBuffer()));
    
        int m_RQPos = CommandLine.Pos(' ');
        if (m_RQPos > 0) {
            CommandLine.Left(m_RQPos, &m_RequestMethodString);
            CommandLine.Delete(0, m_RQPos + 1);            
            m_InputType = StrToHTTPIoType(m_RequestMethodString);
        }
    
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - server request - request method [%s].", m_RequestMethodString.GetBuffer()));    
        
        // no request method, someone is sending junk
        if (m_InputType == htUnknown) {
            Trace(tagHttp, levError, ("CHttpIo::ProcessMethod - server request - unknown request method."));
            return false;
        }
        
        int m_RVPos = CommandLine.InvPos(' ');
        if (m_RVPos >= 0) {
            CommandLine.Right(CommandLine.GetLength() - m_RVPos, &m_RequestVersion);
            CommandLine.Delete(m_RVPos, CommandLine.GetLength());
        }
        
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - server request - url [%s].", CommandLine.GetBuffer()));
        
        m_RequestUrl.SetUrl(CommandLine);
        
    } else {
    
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - cgi request."));
    
        /* CGI request */
        m_RequestMethodString = getenv(g_strCgiRequestMethod);
    
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - cgi request - request method [%s].", m_RequestMethodString.GetBuffer()));    
        
        m_InputType = StrToHTTPIoType(m_RequestMethodString);
     
        CString RequestUrlString = CString("http://") + CString(getenv(g_strCgiServerName));
        CString RequestPort = getenv(g_strCgiServerPort);
        if (CString::StrToInt(RequestPort) != 80)
            RequestUrlString += (':' + RequestPort);
        
        RequestUrlString += CString(getenv(g_strCgiScriptName));
        CString RequestQueryString = getenv(g_strCgiQueryString);
        if (RequestQueryString.GetLength()) 
            RequestUrlString += ('?' + RequestQueryString);
        
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessMethod - cgi request - request url [%s].", RequestUrlString.GetBuffer()));
        
        m_RequestUrl.SetUrl(RequestUrlString);
        
        if (m_InputType == htUnknown) {
            Trace(tagHttp, levError, ("CHttpIo::ProcessMethod - cgi request - unknown request type."));                
            return false;
        }
    }
        
    return true;
}

bool CHttpIo::ProcessOptions(void) {
    if (m_Connection) {
    
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessOptions - server request."));
    
        /* server request */
        CString CommandLine;
        m_Connection->ReadLine(&CommandLine);
        while (CommandLine.GetLength()) {
    
            Trace(tagHttp, levInfo, ("CHttpIo::ProcessOptions - read [%s].", CommandLine.GetBuffer()));
            
            CStringTable Table;
            CString Name, Value;
            if (CMimeParser::ParseLine(CommandLine, Name, Value, Table)) {
                m_HttpFields.Add(Name, Table);
                if (Name == g_strHttpConnection) {
                    m_KeepAlive = Value.Same(g_strHttpKeepAlive);
                }
            }
            m_Connection->ReadLine(&CommandLine);
        }
        
        // for (register int i=0;i<(int)m_HttpFields.GetSize(); i++)
        // for (register int j=0;j<(int)m_HttpFields[i].GetSize();j++)
        // cout << m_HttpFields[i].GetNameAt(j) << "=" << m_HttpFields[i].GetValueAt(j) << endl;        
        
        // host: will adjust the URL
        if (m_RequestVersion.GetLength()) {
            CString Host = m_HttpFields.FindElement(g_strHttpHost).GetValue(g_strHttpHost);            
            if (Host.GetLength() && m_RequestUrl.GetBrute().StartsWith("/")) {
                m_RequestUrl.SetUrl(g_strHttpSpecific + Host + m_RequestUrl.GetBrute());
                Trace(tagHttp, levInfo, ("CHttpIo::ProcessOptions - adjusted url [%s].", m_RequestUrl.GetHttpAll().GetBuffer()));
            }
        }
        
    } else {
    
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessOptions - cgi request."));
    
        /* CGI request */
        CString Variable, Name, Value; int i=0, EPos;
        while (_environ[i]) {
            Variable = _environ[i++];
            if ((EPos = Variable.Pos('=')) > 0) {
                Variable.Mid(0, EPos, &Name);
                Variable.Mid(EPos+1, Variable.GetLength(), &Value);
                if (Name.StartsWithSame(g_strHttpPrefix)) 
                    Name.Delete(0, base_strlen(g_strHttpPrefix));
                if (Name.GetLength()) {
                    Name.Replace('_','-');
                    Name.LowerCase();
                    Name[0] = toupper(Name[0]);                    
                    CStringTable Table;
                    CString NewName, NewValue;
                    if (CMimeParser::ParseLine(Name + ": " + Value, NewName, NewValue, Table)) {
                        /*
                          WriteLine(NewName + "<blockquote>");
                          for (register int i=0;i<(int) Table.GetSize();i++) {
                          WriteLine(Table.GetNameAt(i) + '=' + Table.GetValueAt(i) + "<br>");
                          }
                          WriteLine("</blockquote>");
                        */
                        m_HttpFields.Add(NewName, Table);
                    }
                }
            }
        }
    }
    // cookies:
    ProcessCookies();	
    return true;
}

bool CHttpIo::ProcessRequest(void) {
    
    if (!ProcessMethod()) 
        return false;
    
    if (!ProcessOptions()) 
        return false;    
    
    switch(m_InputType) {
    case htPost:
        ProcessGetMethod();
        if (!ProcessPostMethod()) 
            return false;
        break;
    case htGet:
        if (!ProcessGetMethod()) 
            return false;
        break;
    default:
        break;
    }    
    return true;
}

bool CHttpIo::ProcessGetMethod(void) {
    Trace(tagHttp, levInfo, ("CHttpIo::ProcessGetMethod."));    
    CString Argument = m_RequestUrl.GetHttpSearchPath();
    Argument.Replace('+',' ');
    CVector<CString> AndVector;
    CString::StrToVector(Argument, '&', &AndVector);
    CString MidStringOne, MidStringTwo;
    for (register int i=0;i<(int) AndVector.GetSize();i++) {
        int sPos = AndVector[i].Pos('=');
        if (sPos >= 0) {
            AndVector[i].Mid(0, sPos, &MidStringOne);
            AndVector[i].Mid(sPos + 1, AndVector[i].GetLength(), &MidStringTwo);
            m_GetFields.Set(
                CUrl::UnEscape(MidStringOne),
                CUrl::UnEscape(MidStringTwo));
        } else {
            m_GetFields.Set(
                CUrl::UnEscape(AndVector[i]), 
                CUrl::UnEscape(AndVector[i]));
        }
    }
    return true;
}

bool CHttpIo::ProcessPostMethod(void) {
    Trace(tagHttp, levInfo, ("CHttpIo::ProcessPostMethod."));    
    if (m_HttpFields.FindElement(g_strHttpContentType).GetValue(g_strHttpContentType).Same(g_strHttpEncApplicationXWWWFormUrlEncoded)) {
        // POST, normal form
        return ProcessPost_Application_x_www_form_urlencoded();
    } else if (m_HttpFields.FindElement(g_strHttpContentType).GetValue(g_strHttpContentType).Same(g_strHttpEncMultipartFormData)) {
        // POST, file with boundaries
        return ProcessPost_Multipart_form_data();
    }
    return true;
}

bool CHttpIo::ProcessPost_Multipart_form_data(void) {
    Trace(tagHttp, levInfo, ("CHttpIo::ProcessPost_Multipart_form_data."));    
    CString MultipartData;
    RemoteRead(
        &MultipartData, 
        CString::StrToInt(m_HttpFields.FindElement(g_strHttpContentLength).GetValue(g_strHttpContentLength)));
    return CMimeParser::ProcessMultipart(
        MultipartData, // Raw data
        m_HttpFields.FindElement(g_strHttpContentType).GetValue(g_strHttpBoundary), // Boundary
        m_PostFields);
}

bool CHttpIo::ProcessPost_Application_x_www_form_urlencoded(void) {    
    
    Trace(tagHttp, levInfo, ("CHttpIo::ProcessPost_Application_x_www_form_urlencoded."));    
    
    int nClientDataLength = CString::StrToInt(
        m_HttpFields.FindElement(g_strHttpContentLength).GetValue(g_strHttpContentLength));
    
    if (nClientDataLength <= 0)
        return true;
    
    CString ClientData;
    RemoteRead(&ClientData, nClientDataLength);
        
    CVector<CString> ClientDataVector;
    CString::StrToVector(ClientData, '&', &ClientDataVector);
    CString MidStringOne, MidStringTwo;
    for (register int i=0;i<(int) ClientDataVector.GetSize();i++) {
        ClientDataVector[i].Replace('+',' ');
        int sPos = ClientDataVector[i].Pos('=');
        if (sPos >= 0) {
            ClientDataVector[i].Mid(0, sPos, &MidStringOne);
            ClientDataVector[i].Mid(sPos + 1, ClientDataVector[i].GetLength(), &MidStringTwo);
            CString Name = CUrl::UnEscape(MidStringOne);
            CString Value  = CUrl::UnEscape(MidStringTwo);
            Trace(tagHttp, levInfo, ("CHttpIo::ProcessPost_Application_x_www_form_urlencoded [%s]=[%s].", Name.GetBuffer(), Value.GetBuffer()));    
            CStringTable Table;
            Table.Add(g_strName, Name);
            Table.Add(g_strValue, Value);
            m_PostFields.Add(Name, Table);
        } else {
            Trace(tagHttp, levInfo, ("CHttpIo::ProcessPost_Application_x_www_form_urlencoded - junk on input [%s].", ClientDataVector[i].GetBuffer()));
            // we've got a problem, someone is sending junk?
            return false;
        }
    }
    
    return true;
}

/* Cookies */

const CCookie * CHttpIo::GetCookie(const CString& Name) const {
    CIterator CookieIterator;
    const CCookie * Current = ((CList<CCookie>&)m_Cookies).GetFirst(CookieIterator);
    while (Current) {
        if (Current->GetName() == Name)
            return Current;
        Current = ((CList<CCookie>)m_Cookies).GetNext(CookieIterator);
    }
    return NULL;
}

bool CHttpIo::SetCookie(const CString& Name, const CString& Value) {
    const CCookie * Current = GetCookie(Name);
    if (Current) {
        ((CCookie *) Current)->SetValue(Value);
        ((CCookie *) Current)->SetChanged(true);
        ((CCookie *) Current)->SetPath(m_RequestUrl.GetHttpDirectory());
        return true;
    } else {
        CCookie NewCookie(Name, Value);
        NewCookie.SetChanged(true);
        NewCookie.SetPath(m_RequestUrl.GetHttpDirectory());
        m_Cookies.Add(NewCookie);
        return false;
    }
}

bool CHttpIo::RemoveCookie(const CString& Name) {
    CIterator CookieIterator;
    int CurrentIndex = 0;
    const CCookie * Current = m_Cookies.GetFirst(CookieIterator);
    while (Current) {
        if (Current->GetName() == Name) {
            //m_Cookies.RemoveAt(CurrentIndex);
            ((CCookie *) Current)->SetValue(CString::EmptyCString);
            ((CCookie *) Current)->SetChanged(true);
            return true;
        }
        CurrentIndex++;
        Current = m_Cookies.GetNext(CookieIterator);
    }
    return false;
}

bool CHttpIo::ProcessCookies(void) {    
    CStringTable CookiesTable = m_HttpFields.FindElement("Cookie");
    for (register int i=0;i<(int)CookiesTable.GetSize();i++) {
        CStringPair Pair = CookiesTable[i];
        Trace(tagHttp, levInfo, ("CHttpIo::ProcessCookies - [%s]=[%s].", Pair.GetName().GetBuffer(), Pair.GetValue().GetBuffer()));    
        CCookie NewCookie(Pair.GetName(), Pair.GetValue());
        NewCookie.SetPath(m_RequestUrl.GetHttpDirectory());        
        m_Cookies.Add(NewCookie);
    }    
    return true;
}

void CHttpIo::Flush(void) {    

    Trace(tagHttp, levInfo, ("CHttpIo::Flush at HTTP/1.1 %d, %d bytes.", m_RStatus, m_Buffer.GetLength()));    
    
    if (m_Malformed) {        
        return;
    }
    
    if (!m_Buffer.GetLength()) {
        switch(m_RStatus) {
        case 200: 
            break;
        case 304: // not modified
            m_Buffer.Empty();
            break;
        default:
            m_Buffer = "<html>\n<head><title>Server Error</title></head>\n"       \
                "<body bgcolor=\"#FFFFFF\"><table cellpadding=2 width=400><tr><td>" \
                "<font face=\"Arial, Helvetica, Sans-Serif\" size=\"2px\"><h3>\n"
                + GetRStatusString(m_RStatus) + 
                "</h3></font><hr size=1></td></tr><tr><td>\n"                                                               \
                "<font face=\"Arial, Helvetica, Sans-Serif\" size=\"2px\">"                                                 \
                "The page you are looking for is currently unavailable. "                                                   \
                "The server might be experiencing technical difficulties, "                                                 \
                "you have made an incorrect query, or you may need to adjust your browser settings. "                       \
                "This can also be caused by a mistake in the server configuration. "
                + m_ErrorFooter +
                "</font></td></tr></table></body>\n"                                                                        \
                "</html>\n";
            break;
        };
    }
 
    if (!m_SendHeaders) {
        m_TotalTraffic.Inc(m_Buffer.GetLength() / KBYTE);
        if (m_Connection) {
            m_Connection->Write(m_Buffer);
        } else { 
            fwrite(m_Buffer.GetBuffer(), m_Buffer.GetLength(), 1, stdout);
        }
    } else {        
        CString FullBuffer;
        /* resize approximately to the required buffer size */
        FullBuffer.SetSize(128 +
                           m_Cookies.GetSize() * 32 +
                           m_RFields.GetSize() * 32 +
                           m_Buffer.GetLength());
        /* write response header */
        
        if (m_Connection) {
            FullBuffer += g_strHttpOneOneSpace;
            FullBuffer += GetRStatusString(m_RStatus);
            FullBuffer += g_strCrLf;
        }
        /* cookies */
        CIterator CookieIterator;
        const CCookie * Current = m_Cookies.GetFirst(CookieIterator);
        while (Current) {
            if (Current->GetChanged()) 
                m_RFields.Add(g_strHttpSetCookie, Current->GetSetCookieString());
            Current = m_Cookies.GetNext(CookieIterator);
        }
        /* content-length */
        if (m_SendContentLength)
            m_RFields.Set(g_strHttpContentLength, CString::IntToStr(m_Buffer.GetLength()));
        /* keep-alive */
        if (m_KeepAlive) {
            m_RFields.Set(g_strHttpConnection, g_strHttpKeepAlive);
        }
        /* other fields */
        for (register int i=0;i<(int) m_RFields.GetSize();i++) {            
			Trace(tagHttp, levInfo, ("CHttpIo::Flush - write [%s: %s].", m_RFields.GetNameAt(i).GetBuffer(), m_RFields.GetValueAt(i).GetBuffer()));
            FullBuffer += m_RFields.GetNameAt(i);
            FullBuffer += g_strColSpace;
            FullBuffer += m_RFields.GetValueAt(i);
            FullBuffer += g_strCrLf;
        }
        FullBuffer += g_strCrLf;    
        /* write output buffer */
        m_TotalTraffic.Inc(m_Buffer.GetLength() / KBYTE);
    
        if (m_Buffer.GetLength() < KBYTE * 20) {        
            FullBuffer += m_Buffer;
            if (m_Connection) {
                m_Connection->Write(FullBuffer);
            } else {
                fwrite(FullBuffer.GetBuffer(), FullBuffer.GetLength(), 1, stdout);
            }
        } else {
            if (m_Connection) {
                m_Connection->Write(FullBuffer);
                m_Connection->Write(m_Buffer);
            } else {
                fwrite(FullBuffer.GetBuffer(), FullBuffer.GetLength(), 1, stdout);
                fwrite(m_Buffer.GetBuffer(), m_Buffer.GetLength(), 1, stdout);
            }
        }
    }    
}

CHttpIo::~CHttpIo(void) {
    Flush();
}

CString CHttpIo::GetRStatusString(const int Status) {
    switch (Status) {
    case 100: return g_strHttpStatus100; // "100 Continue
    case 101: return g_strHttpStatus101; // 101 Switching Protocols
    case 200: return g_strHttpStatus200; // 200 OK
    case 201: return g_strHttpStatus201; // 201 Created
    case 202: return g_strHttpStatus202; // 202 Accepted
    case 203: return g_strHttpStatus203; // 203 Non-Authoritative Information
    case 204: return g_strHttpStatus204; // 204 No Content
    case 205: return g_strHttpStatus205; // 205 Reset Content
    case 206: return g_strHttpStatus206; // 206 Partial Content
    case 207: return g_strHttpStatus207; // 207 Partial Content-Length
    case 300: return g_strHttpStatus300; // 300 Multiple Choices
    case 301: return g_strHttpStatus301; // 301 Moved Permanently
    case 302: return g_strHttpStatus302; // 302 Moved Temporarily
    case 303: return g_strHttpStatus303; // 303 See Other
    case 304: return g_strHttpStatus304; // 304 Not Modified
    case 305: return g_strHttpStatus305; // 305 Use Proxy
    case 307: return g_strHttpStatus307; // 307 Temporary Redirect   //HTTP 1.1 Section 10.3.8
    case 400: return g_strHttpStatus400; // 400 Bad Request
    case 401: return g_strHttpStatus401; // 401 Unauthorized
    case 402: return g_strHttpStatus402; // 402 Payment Required
    case 403: return g_strHttpStatus403; // 403 Forbidden
    case 404: return g_strHttpStatus404; // 404 Not Found
    case 405: return g_strHttpStatus405; // 405 Method Not Allowed
    case 406: return g_strHttpStatus406; // 406 Not Acceptable
    case 407: return g_strHttpStatus407; // 407 Proxy Authentication Required
    case 408: return g_strHttpStatus408; // 408 Request Time-out
    case 409: return g_strHttpStatus409; // 409 Conflict
    case 410: return g_strHttpStatus410; // 410 Gone
    case 411: return g_strHttpStatus411; // 411 Length Required
    case 412: return g_strHttpStatus412; // 412 Precondition Failed
    case 413: return g_strHttpStatus413; // 413 Request Entity Too Large
    case 414: return g_strHttpStatus414; // 414 Request-URI Too Large
    case 415: return g_strHttpStatus415; // 415 Unsupported Media Type
    case 416: return g_strHttpStatus416; // 416 Requested range not satisfiable   //HTTP 1.1 Section 10.4.17
    case 417: return g_strHttpStatus417; // 417 Expectation Failed   //HTTP 1.1 Section 10.4.18
    case 500: return g_strHttpStatus500; // 500 Internal Server Error
    case 501: return g_strHttpStatus501; // 501 Not Implemented
    case 502: return g_strHttpStatus502; // 502 Bad Gateway
    case 503: return g_strHttpStatus503; // 503 Service Unavailable
    case 504: return g_strHttpStatus504; // 504 Gateway Time-out
    case 505: return g_strHttpStatus505; // 505 HTTP Version Not Supported
    case 921: return g_strHttpStatus921; // 921 Socket Error
    case 922: return g_strHttpStatus922; // 921 Connection Timed Out
    case 923: return g_strHttpStatus923; // 923 Domain Name Resolution Failed
    case 924: return g_strHttpStatus924; // 924 Socket Bind Error
    case 925: return g_strHttpStatus925; // 925 TLI error
    case 926: return g_strHttpStatus926; // 926 DNS queue full
    case 927: return g_strHttpStatus927; // 927 DNS request timed out
    default: break;
    };
    return CString::IntToStr(Status) + g_strHttpStatusUnsupported;
}

bool CHttpIo::PostReplace(const CString& Source, const CString& Target) {
    CStringTable Table;
    if (m_PostFields.FindAndCopy(Source, Table)) {
        Table.Set(g_strValue, Target);
        m_PostFields.Set(Source, Table);
        return true;
    } else return false;
}

bool CHttpIo::PostGetContains(const CString& Group, const CString& Value) const {
    CStringTable Table;
    if (m_PostFields.FindAndCopy(Group, Table)) {
        return (Table.Contains(Value));
    } else return false;
}

bool CHttpIo::PostGetCopy(const CString& Group, const CString& Value, CString& Target) const {
    CStringTable Table;
    if (m_PostFields.FindAndCopy(Group, Table)) {
        return (Table.FindAndCopy(Value, Target));
    } else return false;
}

void CHttpIo::PostSetNameValue(const CString& Name, const CString& Value) {
    CStringTable Table;
    Table.Set(g_strName, Name);
    Table.Set(g_strValue, Value);
    m_PostFields.Set(Name, Table);
}

int CHttpIo::RemoteRead(CString * pResult, int Size) const {
    if (m_Connection) 
        return m_Connection->Read(pResult, Size);
    else {
        pResult->Empty();
        /* hack: windows NT needs a setmode before you can reopen stdin in binary mode */
#ifdef _WIN32
        _setmode(base_fileno(stdin), O_BINARY);
#endif
        /* hack: stdin opened in text mode, but we're uploading binary data */
        FILE * StandardInput = base_fdopen(0, "rb");
        char * Segment = ::new char[Size+1];
        Size = base_fread(Segment, sizeof(char), Size, StandardInput);
        Segment[Size] = 0;
        pResult->Append(Segment, Size);
        delete[] Segment;
        base_fclose(StandardInput);
        return pResult->GetLength();
    }
}

void CHttpIo::SendFileContents(const CString& Filename, const CString& Contents) {    

    Trace(tagHttp, levInfo, ("CHttpIo::SendFileContents - %s.", Filename.GetBuffer()));

    if (!Contents.GetLength())
        return;
  
    for (int i = 0; i < (int) (sizeof(g_HttpIoFileTypes) / sizeof(CHttpIoFileTypes)); i++)
        if (Filename.EndsWithSame(g_HttpIoFileTypes[i].Extension)) {
            m_RFields.Set("Content-type", g_HttpIoFileTypes[i].MimeType);            
            break;
        }
  
    m_Buffer.Append(Contents);
}
 

void CHttpIo::Send(const CString& Filename) {

	Trace(tagHttp, levInfo, ("CHttpIo::Send - %s.", Filename.GetBuffer()));

    SetRStatus(404);
  
    // support If-Modified-Since
    struct_stat LocalFileStat;
    if (!CLocalFile::GetFileStat(Filename, LocalFileStat))
        return;
  
    struct tm LastModifiedStruct;
    // get the current file modification time (GMT)
    base_gmtime(LocalFileStat.st_mtime, LastModifiedStruct);
    CDate FileModificationDate(&LastModifiedStruct);  
    CString LastModifiedString = FileModificationDate.Map("$dayeng, $day $montheng $year $hour:$min:$sec GMT");
  
    // get the time from the IfModifiedSince header
    CString IfModifiedSinceHeader = m_HttpFields.FindElement(g_strHttpIfModifiedSince).GetValue(g_strHttpIfModifiedSince);
    if (IfModifiedSinceHeader.GetLength()) {
        if (CDate::CompareDates(IfModifiedSinceHeader, LastModifiedString) >= 0) {
            SetRStatus(304);
            return;
        }
    }
  
    // set the last-modified header for future if-Modified-Since
    m_RFields.Set(g_strHttpLastModified, LastModifiedString);
  
    CMMapFile FastFile(Filename);

    if (! FastFile.MMap(MMAP_READOPENMODE))
      return;
    
    long fSize = FastFile.GetSize();
    if (!fSize || !FastFile.GetMem())
        return;

	SetRContentType(Filename);

    SetRStatus(200);
    m_Buffer.Append((char *) FastFile.GetMem(), fSize);
}

bool CHttpIo::SetRContentType(const CString& Filename) {
	for (int i = 0; i < (int) (sizeof(g_HttpIoFileTypes) / sizeof(CHttpIoFileTypes)); i++) {
        if (Filename.EndsWithSame(g_HttpIoFileTypes[i].Extension)) {
            m_RFields.Set("Content-type", g_HttpIoFileTypes[i].MimeType);            
            return true;
        }
    }
	return false;
}

bool CHttpIo::GetUsernamePassword(CString& Username, CString& Password) {
    static const CString BasicPrefix("BASIC ");  
    Username.Empty();
    Password.Empty();
    // for (register int i=0;i<(int)m_HttpFields.GetSize(); i++)
    // for (register int j=0;j<(int)m_HttpFields[i].GetSize();j++)
    // cout << m_HttpFields[i].GetNameAt(j) << "=" << m_HttpFields[i].GetValueAt(j) << endl;  
    CString AuthorizationString;
    if (! m_Connection) {
        AuthorizationString = CString(getenv("HTTP_AUTHORIZATION"));
    } else {
        AuthorizationString = m_HttpFields.FindElement("Authorization").GetValue("Authorization");
    }
    if (!AuthorizationString.GetLength())
        return false;  
    if (!AuthorizationString.StartsWithSame(BasicPrefix))
        return false;  
    AuthorizationString.Delete(0, BasicPrefix.GetLength());
    CString DecodedString = CEncryption::Base64Decode(AuthorizationString);
    int ColPos = DecodedString.Pos(':');
    if (ColPos == -1)
        return false;
    DecodedString.Mid(0, ColPos, &Username);
    DecodedString.Mid(ColPos + 1, DecodedString.GetLength(), &Password);
    return true;
}

void CHttpIo::DenyAccess(const CString& Realm) {
    m_RFields.Set("WWW-Authenticate", "BASIC realm=" + Realm);
    m_RStatus = 401;

    if (! m_Connection)
        m_RFields.Set("Status", "401 Access Denied");
    
    return;
}

bool CHttpIo::PopulateXmlNode(CXmlTree& Tree, CTreeElement< CXmlNode > * pXmlNode) {
    bool bResult = true;
 
    CXmlNode NewElement;
    NewElement.SetType(xmlnOpen);
  
    CTreeElement< CXmlNode > * Node;
    CTreeElement< CXmlNode > * TopNode;

    NewElement.SetData("?xml version=1.0?");  
    TopNode = Tree.AddChildLast(pXmlNode, NewElement);  
    NewElement.SetData("form");
    TopNode = Tree.AddChildLast(TopNode, NewElement);  
    NewElement.SetType(xmlnClose);
    Tree.AddAfter(TopNode, NewElement);  
    
    for (register int i=0; i < PostGetSize(); i++) {
        NewElement.SetType(xmlnOpen);
        NewElement.SetData(PostGetNameAt(i));
        Node = Tree.AddChildLast(TopNode, NewElement);        
        NewElement.SetType(xmlnClose);
        Tree.AddChildLast(TopNode, NewElement);

        NewElement.SetType(xmlnData);
        NewElement.SetData(PostGetValueAt(i));
        Tree.AddChildLast(Node, NewElement);
    
    }
 
    return bResult;
}

void CHttpIo::Initialize(void) {
    
    m_InputType = htUnknown;
    m_RStatus = 200;
    m_RFields.Set(g_strHttpContentType, g_strHttpTextHtml);
    m_BufferLock = false;
    m_SendHeaders = true;
    m_SendContentLength = true;
    m_KeepAlive = false;
    m_Malformed = !ProcessRequest();
    m_ErrorFooter = "<hr size=1><font size=\"1px\">&copy; 1994-2000 <a href=http://www.vestris.com/ target=_new>Vestris Inc.</a>, All Rights Reserved</font>";
    
}

CString CHttpIo::GetDefaultDocument(void) const {

	CString DefaultDocument("index");
	CString DefaultExtension;

	CVector<CString> AcceptArray;

	CString::StrToVector(m_HttpFields.FindElement(g_strHttpAccept).GetValue(g_strHttpAccept), ',', & AcceptArray);

    Trace(tagHttp, levInfo, ("CHttpIo::GetDefaultDocument - %d Accept content-types.", AcceptArray.GetSize()));

	int i, j;

	for (i = 0; i < (int) AcceptArray.GetSize(); i++) {
		int ePos = AcceptArray[i].Pos(';');
		if (ePos >= 0) {
			AcceptArray[i].Delete(ePos, AcceptArray[i].GetLength());
		}
		AcceptArray[i].Trim32();
	}

	for (i = 0; i < (int) (sizeof(g_HttpIoFileTypes) / sizeof(CHttpIoFileTypes)); i++) {
		for (j = 0; j < (int) AcceptArray.GetSize(); j++) {
			if (((g_HttpIoFileTypes[i].MimeContents & hftDefault) > 0) && 
				AcceptArray[j].Same(g_HttpIoFileTypes[i].MimeType)) 
			{
				DefaultDocument += g_HttpIoFileTypes[i].Extension;
				Trace(tagHttp, levInfo, ("CHttpIo::GetDefaultDocument - returning %s for %s.", DefaultDocument.GetBuffer(), AcceptArray[j].GetBuffer()));
				return DefaultDocument;
			}
        }
	}

	DefaultDocument += ".html";
	return DefaultDocument;   
}

bool CHttpIo :: GetContentTypeExtension(const CString& ContentType, const CHttpIoFileTypes ** pType) {
	
	if (! pType)
		return false;

	* pType = NULL;

	for (int i = 0; i < (int) (sizeof(g_HttpIoFileTypes) / sizeof(CHttpIoFileTypes)); i++) {
        if (ContentType.Same(g_HttpIoFileTypes[i].MimeType)) {
            * pType = & g_HttpIoFileTypes[i];
			return true;
        }
	}

	return false;
}

