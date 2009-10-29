/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    * Process multiple Set-Cookie headers in a single server response.
    * Instances of the same path and name will overwrite each other, with the latest instance taking precedence.
    * Instances of the same path but different names will add additional mappings.
    * Setting the path to a higher-level value does not override other more specific path mappings.
    * If there are multiple matches for a given cookie name, but with separate paths, all the matching cookies will be sent.
    * [partially compliant] Expired cookies are purged when requested.
    * [non compliant] Cookies are sent out in the order they have arrived, not in the more specific path mapping order.
    * [non compliant] There're no limitations of the number of cookies.
    * [non compliant] There're no limitations of the size of cookies.
    
    Revision history:

*/

#include <baseclasses.hpp>

#include "CookieStorage.hpp"
#include <Internet/MimeParser.hpp>
#include <String/GStrings.hpp>

CCookieStorage::CCookieStorage(void) {
  
}

CCookieStorage::~CCookieStorage(void) {
  
}

CCookieStorage::CCookieStorage(const CCookieStorage& Storage) {
  operator=(Storage);
}

CCookieStorage& CCookieStorage::operator=(const CCookieStorage& Storage) {
  if (&Storage != this) {    
    m_StorageMutex.Lock();
    m_Cookies = Storage.m_Cookies;
    m_StorageMutex.UnLock(); 
  }
  return * this;
}

ostream& CCookieStorage::operator<<(ostream& Stream) const {
  m_StorageMutex.Lock();
  CIterator CookieIterator;
  const CCookie * Current = m_Cookies.GetFirst(CookieIterator);
  while (Current) {
    Stream << * Current << endl;
    Current = m_Cookies.GetNext(CookieIterator);
  }
  m_StorageMutex.UnLock();
  return Stream;
}

CString CCookieStorage::GetCookieString(const CUrl& Url) const {
  if (!m_Cookies.GetSize()) 
    return CString::EmptyCString;

  m_StorageMutex.Lock();
  CIterator CookieIterator;
  const CCookie * Current = m_Cookies.GetFirst(CookieIterator);
  CString Result;
  while (Current) {
    if (Current->IsSendable(Url)) {
      if (Result.GetLength()) Result += "; ";
      Result += Current->GetString();
    }
    Current = m_Cookies.GetNext(CookieIterator);
  }
  m_StorageMutex.UnLock();
  return Result;
}

bool CCookieStorage::Add(const CStringTable& StringTable, const CUrl * Url) {
  CCookie Cookie;
  if (Url) {
    Cookie.SetDomain(Url->GetDomain());
    Cookie.SetPath(Url->GetHttpDirectory());
  }
  bool Valid = false;
  for (register int i=(int) StringTable.GetSize() - 1;i>=0;i--) {
    if (StringTable[i].GetName().Same(g_strHttpCookieScanSecure)) {
      Cookie.SetSecure(true);
    } else if (StringTable[i].GetName().Same(g_strHttpCookieScanPath)) {
      CString Path = StringTable[i].GetValue();
      if (Url && (Url->GetHttpDirectory().StartsWith(Path))) 
        Cookie.SetPath(Path);      
    } else if (StringTable[i].GetName().Same(g_strHttpCookieScanDomain)) {
      CString Domain = StringTable[i].GetValue();
      if (!Domain.GetLength()) 
        continue;      
      if (Domain[0] != '.') 
        Domain.Insert(0, '.');
      //   Any domain that fails within one of the seven special top level domains listed below
      //   only require two periods. Any other domain requires at least three. The seven special top level domains are:
      //   "COM", "EDU", "NET", "ORG", "GOV", "MIL", and "INT".
      /*
        int ExpectedPeriods = 3;
        if (Domain.EndsWithSame(".com") ||
        Domain.EndsWithSame(".edu") ||
        Domain.EndsWithSame(".net") ||
        Domain.EndsWithSame(".org") ||
        Domain.EndsWithSame(".gov") ||
        Domain.EndsWithSame(".mil") ||
        Domain.EndsWithSame(".int")) 
        ExpectedPeriods = 2;
      */
      if (Domain.GetCount('.') >= 2)
        if (Url && (Url->GetDomain().EndsWith(Domain))) 
          Cookie.SetDomain(Domain);
    } else if (StringTable[i].GetName().Same(g_strHttpCookieScanExpires)) {
      Cookie.SetExpires(StringTable[i].GetValue());
    } else {
      if (StringTable[i].GetName().GetLength()) {	
	Cookie.SetName(StringTable[i].GetName());
	Cookie.SetValue(StringTable[i].GetValue());
	Valid = true;
      }
    }
  }
  /*
    cout << endl << "\tNew cookie: " << Cookie.GetName() << "=" << Cookie.GetValue() << " path=" << Cookie.GetPath() 
    << " domain=" << Cookie.GetDomain() << " expires=" << Cookie.GetTExpires() 
    << " valid=" << CString::BoolToStr(Valid) << endl;
  */
  if (Valid) {
    Remove(Cookie);
    m_StorageMutex.Lock();
    m_Cookies.Add(Cookie);
    m_StorageMutex.UnLock();
  }
  return Valid;
}

bool CCookieStorage::Purge(void) {
  bool Result = false;
  time_t Time; time(&Time);
  m_StorageMutex.Lock();
  for (register int i=m_Cookies.GetSize()-1;i>=0;i--)
    if (m_Cookies[i].IsExpired(&Time)) {
      Result = true;
      m_Cookies.RemoveAt(i);
    }
  m_StorageMutex.UnLock();    
  return Result;
}

bool CCookieStorage::Add(const CString& SetCookieString, const CUrl * Url) {
  CStringTable Table;
  CString Name, Value;
  if (CMimeParser::ParseLine(SetCookieString, Name, Value, Table))
    return Add(Table, Url);
  return false;
}

bool CCookieStorage::Remove(const CCookie& Cookie) {
  bool Result = false;
  time_t Time; time(&Time);
  m_StorageMutex.Lock();
  for (register int i=(int) m_Cookies.GetSize() - 1;i>=0;i--) {
    if (m_Cookies[i].IsExpired(&Time)) {
      m_Cookies.RemoveAt(i);
    } else if ((m_Cookies[i].GetName() == Cookie.GetName()) &&
	       (m_Cookies[i].GetDomain() == Cookie.GetDomain()) &&
	       (m_Cookies[i].GetPath() == Cookie.GetPath()) &&
	       (m_Cookies[i].GetSecure() == Cookie.GetSecure())) {
      Result = true;
      m_Cookies.RemoveAt(i);
      break;
    }
  }
  m_StorageMutex.UnLock();
  return Result;      
}

void CCookieStorage::ProcessHttpRequest(const CHttpRequest& HttpRequest) {
  for (register int i=(int) HttpRequest.GetRFields().GetSize() - 1; i>=0; i--) {
    if (HttpRequest.GetRFields().GetKeyAt(i).Same(g_strHttpSetCookie)) {
      Add(HttpRequest.GetRFields()[i], &HttpRequest.GetUrl());
    }
  }
}
