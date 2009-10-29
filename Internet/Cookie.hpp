/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_COOKIE_HPP
#define BASE_COOKIE_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <Internet/Url.hpp>

//   The Name=Value string is a sequence of characters excluding semi-colon, comma and white space.
//   If there is a need to place such data in the name or value, some encoding method such as URL style
//   %XX encoding is recommended, though no encoding is defined or required.  This is the only required attribute 
//   on the Set-Cookie header. 

//   Changed is an internal flag whether the cookie has changed or not.

//   The expires attribute specifies a date string that defines the valid life time of that cookie. 
//   Once the expiration date has been reached, the cookie will no longer be stored or given out. 
//   The date string is formatted as: Wdy, DD-Mon-YYYY HH:MM:SS GMT
//   This is based on RFC 822, RFC 850, RFC 1036, and RFC 1123, with the variations that the only legal time zone
//   is GMT and the separators between the elements of the date must be dashes. 
//   Expires is an optional attribute. If not specified, the cookie will expire when the user's session ends.

//   When searching the cookie list for valid cookies, a comparison of the domain attributes of the cookie is made with
//   the Internet domain name of the host from which the URL will be fetched. If there is a tail match, then the cookie 
//   will go through path matching to see if it should be sent. "Tail matching" means that domain attribute is matched
//   against the tail of the fully qualified domain name of the host. A domain attribute of "acme.com" would match host
//   names "anvil.acme.com" as well as "shipping.crate.acme.com". Only hosts within the specified domain can set a cookie
//   for a domain and domains must have at least two (2) or three (3) periods in them to prevent domains of the form:
//   ".com", ".edu", and "va.us". Any domain that fails within one of the seven special top level domains listed below
//   only require two periods. Any other domain requires at least three. The seven special top level domains are:
//   "COM", "EDU", "NET", "ORG", "GOV", "MIL", and "INT". 
//   The default value of domain is the host name of the server which generated the cookie response

//   The path attribute is used to specify the subset of URLs in a domain for which the cookie is valid. If a cookie has
//   already passed domain matching, then the pathname component of the URL is compared with the path attribute, and if there
//   is a match, the cookie is considered valid and is sent along with the URL request. The path "/foo" would match "/foobar"
//   and "/foo/bar.html". The path "/" is the most general path. 
//   If the path is not specified, it as assumed to be the same path as the document being described by the header which
//   contains the cookie. 

//   If a cookie is marked secure, it will only be transmitted if the communications channel with the host is a secure one. 
//   Currently this means that secure cookies will only be sent to HTTPS (HTTP over SSL) servers. 
//   If secure is not specified, a cookie is considered safe to be sent in the clear over unsecured channels. 

class CCookie : public CObject {
  property(CString, Name);
  property(CString, Value);
  property(bool, Changed);  
  property(struct tm, SExpires);
  property(time_t, TExpires);
  property(CString, Domain);
  property(CString, Path);
  property(bool, Secure);
public:
  CCookie(void);
  CCookie(const CCookie&);
  CCookie(const CString&, const CString&);
  CCookie& operator=(const CCookie&);
  virtual ~CCookie(void);
  ostream& operator<<(ostream& Stream) const;
  friend ostream& operator<<(ostream&, const CCookie&);
  inline CString GetString(void) const { return m_Name + '=' + m_Value; }
  CString GetSetCookieString(void) const;
  bool IsExpired(time_t * = NULL) const;
  bool IsParent(const CString& Domain, const CString& Path) const;
  bool IsSendable(const CUrl& Url) const;
  bool SetExpires(const CString& Date);
};

inline ostream& operator<<(ostream& Stream, const CCookie& Cookie) {
  return Cookie.operator<<(Stream);
}

#endif
