/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
  Raw mail exchanger (sender only for the moment).

  Revision history:
  
  
*/

#ifndef BASE_EMAIL_EXCHANGER_HPP
#define BASE_EMAIL_EXCHANGER_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <Email/mimemessage.hpp>
#include <Socket/Socket.hpp>

class CEmailExchanger {	
public:
	CEmailExchanger(void);
	virtual ~CEmailExchanger(void);
	static CString SnmpToString(const int SCode);	
	static bool Send(const CString& MailExchanger, CMimeMessage& Email);
private:
	static int ReadLine(const CSocket& Socket, CString& Line);
	static bool WriteLine(const CSocket& Socket, const CString& String, CVector<CString>& StatusVector, int ExpectedResult);
};

#endif
