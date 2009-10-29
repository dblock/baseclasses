/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com
  
    Revision history:
    
*/

#ifndef BASE_COOKIE_STORAGE_HPP
#define BASE_COOKIE_STORAGE_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <Internet/Cookie.hpp>
#include <List/List.hpp>
#include <Vector/Vector.hpp>
#include <Internet/HttpRequest.hpp>
#include <String/StringTable.hpp>
#include <Mutex/Mutex.hpp>

class CCookieStorage : public CObject {
private:
  property(CList<CCookie>, Cookies);
  mutable_property(CMutex, StorageMutex);
public:
  CCookieStorage(void);
  CCookieStorage(const CCookieStorage&);
  virtual ~CCookieStorage(void);
  CCookieStorage& operator=(const CCookieStorage& Storage);
  ostream& operator<<(ostream& Stream) const;
  friend ostream& operator<<(ostream&, const CCookie&);
public:
  inline void RemoveAll(void) { m_Cookies.RemoveAll(); }
  bool Add(const CString& SetCookieString, const CUrl * ParentUrl = NULL);
  bool Add(const CStringTable& StringTable, const CUrl * Url = NULL);
  bool Remove(const CCookie& Cookie);
  bool Purge(void);
  CString GetCookieString(const CUrl& Url) const;
  void ProcessHttpRequest(const CHttpRequest& Request);
  inline bool Add(const CStringTable& StringTable, const CUrl& Url) { return Add(StringTable, & Url); }
  inline bool Add(const CString& SetCookieString, const CUrl& Url) { return Add(SetCookieString, & Url); }
};

inline ostream& operator<<(ostream& Stream, const CCookieStorage& CookieStorage) {
  return CookieStorage.operator<<(Stream);
}


#endif
