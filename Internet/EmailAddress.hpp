/*

	© Vestris Inc., Geneva Switzerland
	http://www.vestris.com, 1998, All Rights Reserved
	__________________________________________________

	written by Daniel Doubrovkine - dblock@vestris.com

	*/

#ifndef BASE_EMAIL_ADDRESS_HPP_RFC822
#define BASE_EMAIL_ADDRESS_HPP_RFC822

#include <platform/include.hpp>
#include <String/String.hpp>

class CEmailAddress : public CObject {
	property(bool, Valid);
	property(CString, Dest);
	property(CString, Mail);
	property(CString, Error);
	property(CString, Domain);
	property(CString, User);
private:
	int In(const char& What, const CString& Where);
	int Parse(void);
	CString MPAddress(const CString& iStr, int& curPos);
	CString MPGroup(const CString& iStr, int& curPos);
	CString MPMailbox(const CString& iStr, int& curPos);
	CString MPPhrase(const CString& iStr, int& curPos);
	CString MPRouteAddr(const CString& iStr, int& curPos);
	CString MPRoute(const CString& iStr, int& curPos);
	CString MPAddressSpec(const CString& iStr, int& curPos);
	CString MPLocalPart(const CString& iStr, int& curPos);
	CString MPWord(const CString& iStr, int& curPos);
	CString MPQuotedString(const CString& iStr, int& curPos);
	char MPQuotedPair(const CString& iStr, int& curPos);
	char MPQText(const CString& iStr, int& curPos);
	CString MPDomain(const CString& iStr, int& curPos);
	CString MPSubDomain(const CString& iStr, int& curPos);
	CString MPDomainRef(const CString&, int&);
	char MPAtom(const CString& iStr, int& curPos);
	CString MPAtomSequence(const CString& iStr, int& curPos);
	char MPDtext(const CString& iStr, int& curPos);
	char MPLinearWhiteSpace(const CString& iStr, int& curPos);
	CString MPDomainLiteral(const CString& iStr, int& curPos);
public:
	CEmailAddress(void);
	CEmailAddress(const CString&);
	virtual ~CEmailAddress(void);
	void Set(const CString&);
};

#endif
