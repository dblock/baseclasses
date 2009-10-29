/*

    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

/*

  [RFC1738]
  "Uniform Resource Locators", T. Berners-Lee, L. Masinter, and M. McCahill, December 1994.
  Available at http://ds.internic.net/rfc/rfc1738.txt.
  http://www.faqs.org/rfcs/rfc1738.html

*/

#include <baseclasses.hpp>

#include "Url.hpp"
#include <String/GStrings.hpp>
#include <Object/Tracer.hpp>
#include <File/LocalPath.hpp>
 
CUrl::CUrl(void) {
	m_Valid = false;
}

CUrl::CUrl(const CString& iUrl){  
        SetUrl(iUrl);
}

CUrl::CUrl(const CUrl& Other) {
	(* this) = Other;
}

CUrl::~CUrl(void) {

}

CUrl& CUrl::operator=(const CUrl& Other) {
	m_Brute = Other.m_Brute;
	m_Scheme = Other.m_Scheme;
	m_Specific = Other.m_Specific;
	m_User = Other.m_User;
	m_Password = Other.m_Password;
	m_Port = Other.m_Port;
	m_Host = Other.m_Host;
	m_UrlPath = Other.m_UrlPath;
	m_HostVector = Other.m_HostVector;
	m_FtpPath = Other.m_FtpPath;
	m_FtpPathType = Other.m_FtpPathType;
	m_HttpPath = Other.m_HttpPath;
	m_HttpSearchPath = Other.m_HttpSearchPath;
	m_HttpArg = Other.m_HttpArg;
	m_HttpSig = Other.m_HttpSig;
	m_HttpDirectory = Other.m_HttpDirectory;
	m_HttpFilename = Other.m_HttpFilename;
	m_Valid = Other.m_Valid;
	return (* this);
}

void CUrl::SetUrl(const CString& iUrl){  
	if (m_Brute.GetLength()) {
		m_Scheme.Empty();
		m_Specific.Empty();
		m_User.Empty();
		m_Password.Empty();
		m_Port.Empty();
		m_Host.Empty();
		m_UrlPath.Empty();
		m_HostVector.RemoveAll();
		m_FtpPath.RemoveAll();
		m_FtpPathType = 0;
		m_HttpPath.Empty();
		m_HttpSearchPath.Empty();
		m_HttpArg.Empty();
		m_HttpSig.Empty();
		m_HttpDirectory.Empty();
		m_HttpFilename.Empty();
	}
	m_Brute = iUrl;
	m_Valid = false;
	ParseUrl(m_Brute);
}

CString CUrl::ReadScheme(int& curPos, const CString& iUrl){
	/*
	  scheme must start at zero, known schemes are
	  (implemented) ftp       File Transfer protocol
	  (implemented) http      Hypertext Transfer Protocol
	  gopher                  The Gopher protocol
	  mailto                  Electronic mail address
	  news                    USENET news
	  nntp                    USENET news using NNTP access
	  telnet                  Reference to interactive sessions
	  wais                    Wide Area Information Servers
	  file                    Host-specific file names
	  prospero                Prospero Directory Service
	*/
	int curPosLocal = 0;
        CString MidString;
	while (curPosLocal < (int) iUrl.GetLength()) {
		switch(iUrl[curPosLocal]) {
		case ':':
			curPos = curPosLocal+1;
                        iUrl.Mid(0, curPosLocal, &MidString);
			return MidString;
		case '+':
		case '.':
		case '-':
			break;
		default:
			if ((iUrl[curPosLocal] >= 'a')&&(iUrl[curPosLocal] <= 'z')) break; 
			else return CString::EmptyCString;
		}
		curPosLocal++;
	}
	return CString::EmptyCString;
}

void CUrl::ParseUrl(const CString& iUrl){
    
    if (!iUrl.GetLength()) 
        return;

    // URL general syntax: <scheme>:<scheme-specific-part>

    int curPos = 0;
    CString MidString;
    m_Scheme = ReadScheme(curPos, iUrl);
    
    // cout << "CUrl() - scheme:   " << m_Scheme << endl;
    
    if (m_Scheme.GetLength()) {

        iUrl.Mid(curPos, 2, & MidString);

        if (MidString.Different("//")) {
            m_Scheme = g_strProto_HTTP;
            curPos = 0;
        } else curPos += 2;

    } else {

		m_Scheme = g_strProto_HTTP;

	}

    iUrl.Mid(curPos, iUrl.GetLength(), & m_Specific);

    if (m_Scheme.Same(g_strProto_FILE) && m_Specific.StartsWith("//")) {
      // unc path
    } else if (m_Specific.StartsWith('/')) {
        // allow no hostname
        // trim whatever slashes left
        m_Specific.TrimLeft('/', '/');	
        // cout << "CUrl() - specific: " << m_Specific << endl;
#ifdef _WIN32
        if (! CLocalPath::IsDrive(m_Specific))
            m_Specific.Insert(0, '/');
#endif
#ifdef _UNIX
        m_Specific.Insert(0, '/');
#endif
    }

	/*

	  <user>:<password>@<host>:<port>/<url-path>
	  parse terms of dilimiters

	*/

    if (m_Scheme.Same(g_strProto_FILE)) {
        
        m_Specific.Mid(0, m_Specific.GetLength(), & m_UrlPath);
        m_Valid = ParseHttpPath(m_UrlPath);

    } else if (ParseUrlSpecific(curPos, m_Specific)) {
        
        if (!m_Host.GetLength()) 
            m_Host = "localhost";            
        if (!m_Port.GetLength()) 
            m_Port = "80";

		// cout << "CUrl() - user:     " << m_User << endl;
		// cout << "CUrl() - password: " << m_Password << endl;
		// cout << "CUrl() - host:     " << m_Host << endl;
		// cout << "CUrl() - port:     " << m_Port << endl;

		if (ParseHost(m_Host)) {
            m_Specific.Mid(curPos, m_Specific.GetLength(), &m_UrlPath);
			// _L_DEBUG(1, cout << "CUrl() - url-path: " << m_UrlPath << endl);
            if (m_Scheme.Same(g_strProto_HTTP) || 
                m_Scheme.Same(g_strProto_HTTPS)) {
                m_Valid = ParseHttpPath(m_UrlPath);
            } else if (m_Scheme.Same(g_strProto_FTP)) {
                m_Valid = ParseFtpPath(m_UrlPath);
            } else {
				m_Valid = false;
				m_Error = "scheme not implemented";
			}
		}
	}
}

bool CUrl::ParseHttpPath(const CString& iUrlPath){

	/*
	  m_HttpDirectory = directory with no parameters, cannot be empty
	  m_HttpFilename = filename with no parameters
	  m_HttpArg = everything except the location inside the page defined by #
	  m_HttpSig = location name defined in the page
	  m_HttpSearchPath = everything after the parameter (?) without the location inside page
	  m_HttpPath = everything before the parameter (?)
	*/

    // cout << "ParseHttpPath:[" << iUrlPath << "]" << endl;

    int sPos = iUrlPath.Pos('#');
    if (sPos >= 0) {
        iUrlPath.Mid(sPos+1, iUrlPath.GetLength(), &m_HttpSig);
        iUrlPath.Mid(0, sPos, &m_HttpArg);
    } else m_HttpArg = iUrlPath;
    
    int qPos = m_HttpArg.Pos('?');
    if (qPos >= 0) {
        m_HttpArg.Mid(0, qPos, &m_HttpPath);
        m_HttpArg.Mid(qPos+1, m_HttpArg.GetLength(), &m_HttpSearchPath);
    } else m_HttpPath = m_HttpArg;
    
    m_HttpPath.Replace('\\','/');

    int ePos = m_HttpPath.InvPos('/');
    if (ePos >= 0) {
        m_HttpPath.Mid(0, ePos, &m_HttpDirectory);
        m_HttpPath.Mid(ePos+1, m_HttpPath.GetLength(), &m_HttpFilename);
    } else {
        m_HttpFilename = m_HttpPath;
        // drive only, this is not a filename
#ifdef _WIN32
	if (m_Scheme.Same(g_strProto_FILE) && m_HttpPath.GetLength() == 2 && m_HttpPath[1] == ':') {
            m_HttpDirectory = m_HttpFilename;
            m_HttpFilename.Empty();
	}
#endif
    }

    if (m_HttpDirectory.GetLength())
      m_HttpDirectory.TerminateWith('/');

    // cannot be relative with a host name
    if (m_Host.GetLength())
      if (! m_HttpDirectory.GetLength() || m_HttpDirectory[0] != '/')
	m_HttpDirectory.Insert(0, '/');      
    
    return true;
}

bool CUrl::ParseFtpPath(const CString& iUrlPath){
	static const CString __Type("type=");
	int curPos = 0;
	int prevPos = 0;
	m_FtpPathType = 0;
        CString MidString;
	while (curPos < (int) iUrlPath.GetLength()) {
		switch(iUrlPath[curPos]){
		case '/':
                        iUrlPath.Mid(prevPos, curPos-prevPos, &MidString);
			// _L_DEBUG(1, cout << "ftp-item: " << MidString << endl);
			m_FtpPath+=MidString;
			prevPos = curPos+1;
			break;
		case ';':
                        iUrlPath.Mid(prevPos, curPos-prevPos, &MidString);
			// _L_DEBUG(1, cout << "ftp-item: " << MidString << endl);
			m_FtpPath+=MidString;
			prevPos = curPos+1;
                        iUrlPath.Mid(prevPos, __Type.GetLength(), &MidString);
			if (MidString.Same(__Type)) {
				curPos+=(__Type.GetLength()+1);
				m_FtpPathType = iUrlPath[curPos];
				// _L_DEBUG(1, cout << "ftp-type: " << m_FtpPathType << endl);
				if ((int) iUrlPath.GetLength() > (curPos+1)){
					m_Error = "FTP type too long";
					return false;
				}
				return true;
			} else {
				m_Error = "invalid FTP type";
				return true;
			}
		}
		curPos++;
	}
	return true;
}

bool CUrl::ParseUrlSpecific(int& curPos, const CString& iUrl){
	curPos = 0;
	int prevPos = 0;
	CString curVector;
	while (curPos < (int) iUrl.GetLength()) {
		switch(iUrl[curPos]){
		case ':':
			if (curVector.GetLength()) {
				m_Error = "invalid host/username";
				return false;
			}
			iUrl.Mid(prevPos, curPos-prevPos, &curVector);
			prevPos=curPos+1;
			break;
		case '@':
			if (curVector.GetLength()) {
				m_User = curVector;
				iUrl.Mid(prevPos, curPos-prevPos, &m_Password);
				curVector.Empty();
			} else iUrl.Mid(prevPos, curPos-prevPos, &m_User);
			prevPos = curPos+1;
			break;
		case '/':
			if (curVector.GetLength()) {
				m_Host = curVector;
				iUrl.Mid(prevPos, curPos-prevPos, &m_Port);
				if (!CString::StrToInt(m_Port)) {
					m_Error = "invalid port";
					return false;
				}
				curVector.Empty();
			} else iUrl.Mid(prevPos, curPos-prevPos, &m_Host);
			curPos++;
			return true;
		}
		curPos++;
	}
	if (curVector.GetLength()) {
		m_Host = curVector;
                iUrl.Mid(prevPos, curPos-prevPos, &m_Port);
	} else iUrl.Mid(prevPos, curPos-prevPos, &m_Host);
	return true;
}

char CUrl::X2C(char * what) {
	char digit;
	digit = (char) (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
	digit *= 16;
	digit += ((what[1] >= 'A') ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
	return(digit);
}

/*
  Encode a string to the "x-www-form-urlencoded" form, enhanced
  with the UTF-8-in-URL proposal. 
*/

#define COPY_HEX(__tmp, __ssc, __hex)           \
      __tmp[0] = '%';                           \
      __tmp[1] = __hex[0];                      \
      __tmp[2] = __hex[1];                      \
      __ssc.Append(__tmp, 3);

CString CUrl::Escape(const CString& Brute, bool Everything) {
  CString sscTmp; 
  sscTmp.SetSize(Brute.GetLength());
  
  static const char * hex[256] = 
  {
      "00", "01", "02", "03", "04", "05", "06", "07",
      "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
      "10", "11", "12", "13", "14", "15", "16", "17",
      "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
      "20", "21", "22", "23", "24", "25", "26", "27",
      "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
      "30", "31", "32", "33", "34", "35", "36", "37",
      "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
      "40", "41", "42", "43", "44", "45", "46", "47",
      "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
      "50", "51", "52", "53", "54", "55", "56", "57",
      "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
      "60", "61", "62", "63", "64", "65", "66", "67",
      "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
      "70", "71", "72", "73", "74", "75", "76", "77",
      "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
      "80", "81", "82", "83", "84", "85", "86", "87",
      "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
      "90", "91", "92", "93", "94", "95", "96", "97",
      "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
      "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
      "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
      "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7",
      "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
      "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7",
      "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
      "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
      "d8", "d9", "da", "db", "dc", "dd", "de", "df",
      "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7",
      "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
      "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
      "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
  };	

  if (Brute.GetLength()) {
    char tmpBuf[3];
    unsigned char tmpCh;
    for (int i=0;i<(int)Brute.GetLength();i++) {
      tmpCh = (int) Brute[i];
      // ASCII characters remain the same
      if (('A' <= tmpCh && tmpCh <= 'Z') ||
          ('a' <= tmpCh && tmpCh <= 'z') ||
          ('0' <= tmpCh && tmpCh <= '9')) {		                    // 'A'..'Z', 'a'..'z', '0'..'9'        
        sscTmp.Append(tmpCh);
        continue;
      } 
      if (!Everything && (tmpCh > ' ') && (tmpCh <= 0x007f)) {
        sscTmp.Append(tmpCh);
        continue;
      }
      /*
        All other ASCII characters are converted into the 3-character string
        "%xy", where xy is the two-digit hexadecimal representation of the
        character code  
      */
      if (tmpCh <= 0x00FF) {
        COPY_HEX(tmpBuf, sscTmp, hex[tmpCh]);
        /*
          All non-ASCII characters are encoded in two steps: first to a
          sequence of 2 or 3 bytes, using the UTF-8 algorithm; secondly each of
          these bytes is encoded as "%xx".
        */
      } else if (tmpCh <= 0x07FF) {
        COPY_HEX(tmpBuf, sscTmp, hex[0xc0 | (tmpCh >> 6)]);
        COPY_HEX(tmpBuf, sscTmp, hex[0x80 | (tmpCh & 0x3F)]);
      } else {
        COPY_HEX(tmpBuf, sscTmp, hex[0xc0 | (tmpCh >> 12)]);
        COPY_HEX(tmpBuf, sscTmp, hex[0x80 | ((tmpCh >> 6) & 0x3F)]);
        COPY_HEX(tmpBuf, sscTmp, hex[0x80 | (tmpCh & 0x3F)]);
      }
    }
  }
  return sscTmp;
}

CString CUrl::UnEscape(const CString& Brute) {
	CString Result;
    Result.SetLength(Brute.GetLength() + 3);
    
    int x, y;
    
    for(x = 0, y = 0; y < (int) Brute.GetLength(); ++x, ++y) {
        Result[x] = Brute[y];
        if ((Result[x] == '%') && (y + 2 < (int) Brute.GetLength())) {
            Result[x] = (unsigned char) X2C((char *) & (Brute.GetBuffer()[y + 1]));
            y += 2;
        }
    }
    
    Result.SetLength(x, 0, false);    
	return Result;
}

bool CUrl::ParseHost(const CString& iHost){
    
	int curPos = 0;
	int prevPos = 0;
        CString MidString;
	while ((curPos < (int) iHost.GetLength()) && (isalnum((unsigned char) iHost[curPos]) || (iHost[curPos]=='-') || (iHost[curPos]=='.'))) {
		switch(iHost[curPos]) {
		case '.':
                  iHost.Mid(prevPos, curPos - prevPos, &MidString);      
                  m_HostVector+=MidString;
                  prevPos = curPos+1;
		}
		curPos++;
	}
        iHost.Mid(prevPos, curPos - prevPos, &MidString);
	m_HostVector+=MidString;
	if (curPos != (int) iHost.GetLength()-1) {
		m_Error = "invalid host";
		return true;
	} else return false;
}

bool CUrl::operator==(const CUrl& iUrl) const {
	return (
		(m_Scheme.Same(iUrl.m_Scheme))&&
		(m_User == iUrl.m_User)&&
		(m_Password == iUrl.m_Password)&&
		(m_Port == iUrl.m_Port)&&
		(m_Host.Same(iUrl.m_Host))&&
		(m_UrlPath == iUrl.m_UrlPath)&&
		(m_FtpPathType == iUrl.m_FtpPathType)&&
		(m_HttpPath == iUrl.m_HttpPath)&&
		(m_HttpArg == iUrl.m_HttpArg)&&
		(m_HttpSig == iUrl.m_HttpSig)&&
		(m_Valid)&&(iUrl.m_Valid));
}

CString CUrl::GetUrlPath(void) const {
	CString Result(m_HttpDirectory);
	Result += m_HttpFilename;
	if (m_HttpSearchPath.GetLength()) {
		Result += '?';
		Result += m_HttpSearchPath;
	}
	return UnEscape(Result);
}

CUrl CUrl::Resolve(const CUrl& iUrl) const {

    CUrl Result(iUrl);

    // cout << "resolving " << GetHttpAll() << " with " << Result.GetHttpAll() << endl;

    if (Result.m_Scheme != m_Scheme) 
        return Result;

    if ((Result.m_Host.Same("localhost")) || (Result.m_Host.GetLength() == 0)) {
        Result.m_Host = m_Host;
    } else return Result;

    Result.m_HttpDirectory = CLocalPath::ResolveDirectory(m_HttpDirectory, Result.m_HttpDirectory);
    Result.m_Brute = Result.GetHttpAll();

    return Result;
}

CUrl CUrl::Resolve(const CString& iUrl) const {

    CUrl Result;
    CString MidString;
    int colPos = iUrl.Pos(':');
    if (colPos >= 0){
        iUrl.Mid(0, colPos, &MidString);
        MidString.UpperCase();
        CString colStart = MidString;
        if ((colStart == g_strProto_HTTP)||
            (colStart == g_strProto_HTTPS)||		    
            (colStart == g_strProto_FILE)||
            (colStart == g_strProto_FTP)||
            (colStart == g_strProto_GOPHER)||
            (colStart == g_strProto_NEWS)||
            (colStart == g_strProto_NNTP)||
            (colStart == g_strProto_TELNET)||
            (colStart == g_strProto_WAIS)||
            (colStart == g_strProto_PROSPERO)
            ) {
            Result.SetUrl(iUrl);
            return Resolve(Result);
        }
    }
    
    Result = (* this);

    if (iUrl.GetLength() && (iUrl[0] == '/')) {
        Result.ParseHttpPath(iUrl);
#ifdef _WIN32
    } else if (m_Scheme.Same(g_strProto_FILE) && CLocalPath::IsDrive(iUrl)) {
        Result.ParseHttpPath(iUrl);
#endif
    } else if (iUrl.Pos(':') == -1) {
		Result.ParseHttpPath(CLocalPath::ResolveDirectory(m_HttpDirectory, iUrl));
    } else {
        CUrl NewResult;
        NewResult.m_HttpDirectory = iUrl;
        NewResult.m_Brute = NewResult.GetHttpAll();
        return NewResult;
    }
    Result.m_Brute = Result.GetHttpAll();
    return Result;
}

CString CUrl::GetHttpServer(void) const {
    
    CString Result(m_Scheme);

    if (Result.GetLength()) {
        Result += g_strColSlashSlash;

        if (m_Scheme.Same(g_strProto_HTTP) || m_Scheme.Same(g_strProto_HTTPS)) {
            
            Result += m_Host;
            int DefaultPort = 80;
            
            if (m_Scheme.Same(g_strProto_HTTPS)) {
                DefaultPort = 443;
            }

            if (CString::StrToInt(m_Port) != DefaultPort) {
                Result += ':';
                Result += GetPort();
            }
        }
    }

    return Result;
}

CString CUrl::GetHttpAll(void) const {
    CString Result;
    
    Result += GetHttpServer();

    Result += m_HttpDirectory;
    Result += m_HttpFilename;
    
    if (m_HttpSearchPath.GetLength()) {
      Result += '?';
      Result += m_HttpSearchPath;
    }
    
    return Result;
}

/*
  Current URL contains the other URL?
*/
int CUrl::Includes(const CUrl& iUrl) const {

	Trace(tagSystem, levVerbose, ("url-includes: scheme: %s - %s", GetScheme().GetBuffer(), iUrl.GetScheme().GetBuffer()));
	Trace(tagSystem, levVerbose, ("url-includes: host: %s - %s", GetHost().GetBuffer(), iUrl.GetHost().GetBuffer()));
	Trace(tagSystem, levVerbose, ("url-includes: port: %d - %d", GetPortValue(), iUrl.GetPortValue()));
	
	// compare schemes (http://)
	if (!GetScheme().Same(iUrl.GetScheme())) return 0;
	// compare host names (todo: IPs) and ports
	if (!GetHost().EndsWith(iUrl.GetHost())) return 0;
	if (GetPortValue() != iUrl.GetPortValue()) return 0;
	// everything seems okay
	return 1;
}

CString CUrl::GetDomain(void) const {
  CVector<CString> DomainVector; 
  CString::StrToVector(m_Host, '.', &DomainVector);
  if (DomainVector.GetSize() > 2) 
    DomainVector.RemoveAt(0);
  CString VectorString;
  CString::VectorToStr(DomainVector, '.', &VectorString);
  return UnEscape(VectorString);
}
