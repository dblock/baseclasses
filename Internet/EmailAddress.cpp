/*

    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com
    
    Revision history:
    20.08.1999: accepting email of form "Name <user@host.ext>"

*/

/*

  CEmailAddress - email syntax (maybe semantics)

  RFC 822

  address      =     mailbox                        ; one addressee
               /     group                          ; named list
  group        =     phrase ":" [#mailbox] ";"
  mailbox      =     addr-spec                      ; simple address
               /     phrase route-addr              ; name & addr-spec
  route-addr   =     "<" [route] addr-spec ">"
  route        =     1#("@" domain) ":"             ; path-relative

  addr-spec    =     local-part "@" domain          ; global address
  local-part   =     word *("." word)               ; uninterpreted
                                                    ; case-preserved
  domain       =     sub-domain *("." sub-domain)
  sub-domain   =     domain-ref / domain-literal
  domain-ref   =     atom                           ; symbolic reference

*/

#include <baseclasses.hpp>

#include "EmailAddress.hpp"
#include <String/GStrings.hpp>

#define CR 13
#define LF 10
#define HTAB 9
#define SPACE 32

CEmailAddress::CEmailAddress(void){
	m_Valid = 0;
}

CEmailAddress::~CEmailAddress(void) {

}

CEmailAddress::CEmailAddress(const CString& Mail){
	Set(Mail);
}

void CEmailAddress::Set(const CString& Mail){
	m_Mail = Mail;
	m_Mail.Trim();
	m_Error.Empty();
	int lPos = m_Mail.Pos('<');
	int rPos = m_Mail.InvPos('>');
	if ((lPos < rPos)&&(lPos != -1)&&(rPos != -1)) {
		m_Mail.Mid(0, lPos, &m_Dest); m_Dest.Trim32();
		m_Mail.Mid(lPos+1, rPos - lPos - 1, &m_Mail); m_Mail.Trim32();		
	}
	m_Valid = Parse();
}

int CEmailAddress::Parse(void){
	int curPos=0;
	CString ParseResult = MPAddress(m_Mail, curPos);
	return (((int) ParseResult.GetLength() > 0) && (curPos == (int) m_Mail.GetLength()));
}

CString CEmailAddress::MPAddress(const CString& iStr, int& curPos){
	/*
	  address =  mailbox ; one addressee
	          /  group   ; named list
	*/
	if (curPos < (int) iStr.GetLength()) {
		CString Mailbox = MPMailbox(iStr, curPos);
		if (Mailbox.GetLength()) return Mailbox;
		CString Group = MPGroup(iStr, curPos);
		if (Group.GetLength()) return Group;
		return CString::EmptyCString;
	} else return CString::EmptyCString;
}


CString CEmailAddress::MPGroup(const CString& iStr, int& curPos){
	/*
	  group = phrase ":" [#mailbox] ";"
	*/
	
	CString Result;
	CString Phrase = MPPhrase(iStr, curPos);
	if (!Phrase.GetLength()) return CString::EmptyCString;
	Result+=Phrase;
	if (curPos < (int) iStr.GetLength()) {
		if (iStr[curPos]!=':') return CString::EmptyCString;
		Result+=':';
		int workingPos = curPos+1;
		if (workingPos < (int) iStr.GetLength()) {
			CString MBox;
			if (iStr[workingPos]!=',') do {
				if (iStr[workingPos] == ',') {
					workingPos++;
					Result+=',';
				}
				MBox = MPMailbox(iStr, curPos);
				if (MBox.GetLength()) Result+=MBox; else break;
			} while (iStr[workingPos] == ',');
			if (iStr[workingPos] == ';'){
				Result+=';';
				workingPos++;
				curPos = workingPos;
				return Result;
			} else return CString::EmptyCString;
		} else return CString::EmptyCString;
	} else return CString::EmptyCString;
}


CString CEmailAddress::MPMailbox(const CString& iStr, int& curPos){
	/*
	  mailbox =  addr-spec              ; simple address
	          /  phrase route-addr      ; name & addr-spec
	*/
	
	if (curPos < (int) iStr.GetLength()) {
		CString ASpec = MPAddressSpec(iStr, curPos);
		if (!ASpec.GetLength()) {
			CString Phrase = MPPhrase(iStr, curPos);
			if (Phrase.GetLength()) {
				CString RAddr = MPRouteAddr(iStr, curPos);
				if (RAddr.GetLength()) {
					CString Result = Phrase;
					Result+=RAddr;
					return Result;
				} else return CString::EmptyCString;
			} else return CString::EmptyCString;
		} else return ASpec;
	} else return CString::EmptyCString;
}

CString CEmailAddress::MPPhrase(const CString& iStr, int& curPos){
	/*
	  1*word
	*/
	
	CString Result;
	CString Word;
	Word = MPWord(iStr, curPos);
	while (Word.GetLength()) {
		Result+=Word;
		Word = MPWord(iStr, curPos);
	}
	
	return Result;
}

CString CEmailAddress::MPRouteAddr(const CString& iStr, int& curPos){
	/*
	  route-addr  =  "<" [route] addr-spec ">"
	*/
	
	if (iStr[curPos] == '<') {
		int workingPos=curPos+1;
		CString Route = MPRoute(iStr, workingPos);
		CString AddrSpec = MPAddressSpec(iStr, workingPos);
		if (AddrSpec.GetLength()) {
			CString Result(Route);
			Result+=AddrSpec;
			curPos = workingPos;
			return Result;
		} else return CString::EmptyCString;
	} else return CString::EmptyCString;
}

CString CEmailAddress::MPRoute(const CString& iStr, int& curPos){
	/*
	  route = 1#("@" domain) ":"          ; path-relative
	  means 1 at least "@:" or "domain:"
	  separated eventually by commas
	*/
	
	if (curPos < (int) iStr.GetLength()) {
		int workingPos = curPos;
		if (iStr[workingPos] != '@') {
			CString FDomain = MPDomain(iStr, workingPos);
			if (!FDomain.GetLength()) return CString::EmptyCString;
		} else workingPos++;
		if (iStr[workingPos] != ':') return CString::EmptyCString;
		CString Result;
                iStr.Mid(curPos, workingPos-curPos, &Result);
		curPos = workingPos+1;
		if (iStr[curPos] == ',') {
                  Result+=MPRoute(iStr, curPos);
                  return Result;
		} else return Result;
	} else return CString::EmptyCString;
}

CString CEmailAddress::MPAddressSpec(const CString& iStr, int& curPos){
	/*
	  addr-spec = local-part "@" domain ; global address
	*/
	
	if (curPos < (int) iStr.GetLength()) {
		m_User = MPLocalPart(iStr, curPos);
		if (!m_User.GetLength()) return CString::EmptyCString;
		if (iStr[curPos] == '@') {
			int workingPos = curPos+1;
			m_Domain = MPDomain(iStr, workingPos);
			if (!m_Domain.GetLength()) return CString::EmptyCString;
			CString Result(m_User);
			Result+='@';
			Result+=m_Domain;
			curPos = workingPos;
			return Result;
		} else return CString::EmptyCString;
	} else return CString::EmptyCString;
}

CString CEmailAddress::MPLocalPart(const CString& iStr, int& curPos){
	/*
	  addr-spec    = local-part "@" domain  ; global address
	  local-part   = word *("." word)       ; uninterpreted
	*/
	
	CString Result;
	CString Word;
	int workingPos = curPos;
	Word = MPWord(iStr, workingPos);
	while ((iStr[workingPos] == '.') && Word.GetLength() && (curPos < (int) iStr.GetLength())){
		workingPos++;
		if (Result.GetLength())
                  Result+='.';
		Result+=Word;
		Word = MPWord(iStr, workingPos);
	}
	if (Word.GetLength()) Result+=Word;
	if (Result.GetLength()) curPos = workingPos;
	return Result;
}

CString CEmailAddress::MPWord(const CString& iStr, int& curPos){
	
	CString Result;
	Result = MPAtomSequence(iStr, curPos);
	if (!Result.GetLength()) {
		Result = MPQuotedString(iStr, curPos);
	}
	
	return Result;
}

CString CEmailAddress::MPQuotedString(const CString& iStr, int& curPos){
	/*
	  quoted-string = <"> *(qtext/quoted-pair) <">; Regular qtext or quoted chars.
	*/
	
	if (curPos < (int) iStr.GetLength()) {
		if (iStr[curPos] == '\"'){
			int workingPos = curPos+1;
			while (workingPos < (int) iStr.GetLength()){
				if (!MPQText(iStr, workingPos))
					if (!MPQuotedPair(iStr, workingPos))
						break;
			}
			if (iStr[workingPos] == '\"') {
                                CString Result;
                                iStr.Mid(curPos, workingPos - curPos, &Result);
				curPos = workingPos+1;
				return Result;
			} else return CString::EmptyCString;
		} else return CString::EmptyCString;
	} else return CString::EmptyCString;
}

char CEmailAddress::MPQText(const CString& iStr, int& curPos){
	/*
	  <any CHAR excepting <">,     ; => may be folded
	  "\" & CR, and including
	  linear-white-space>
	*/	

	if (curPos < (int) iStr.GetLength()) {
		char c = iStr[curPos];
		switch(c){
		case '\"':
		case '[':
		case ']':
		case '\\':
		case CR:
			return 0;
		}
		char d;
		if ((d = MPLinearWhiteSpace(iStr, curPos))) return d;
		curPos++;
		return c;
	} else return 0;
}

CString CEmailAddress::MPDomain(const CString& iStr, int& curPos){
	/*
	  domain = sub-domain *("." sub-domain)
	*/
	
	CString Result;
	CString SubDomain;
	int workingPos = curPos;
	SubDomain = MPSubDomain(iStr, workingPos);
	while ((workingPos < (int) iStr.GetLength())&&(iStr[workingPos] == '.') && SubDomain.GetLength()){
		workingPos++;
		if (Result.GetLength()) 
                  Result+='.';
		Result+=SubDomain;
		SubDomain = MPSubDomain(iStr, workingPos);
	}
	if (SubDomain.GetLength()) {
		if (Result.GetLength())
                  Result+='.';
		Result+=SubDomain;
	}
	if (Result.GetLength()) curPos = workingPos;
	
	return Result;
}

CString CEmailAddress::MPSubDomain(const CString& iStr, int& curPos){
	/*
	  sub-domain = domain-ref / domain-literal
	  domain-ref = atom
	*/

	CString Result = MPDomainRef(iStr, curPos);
	if (!Result.GetLength()) Result = MPDomainLiteral(iStr, curPos);	

	return Result;
}

char CEmailAddress::MPAtom(const CString& iStr, int& curPos){
	/*
	  atom = 1*<any CHAR except specials, SPACE and CTLs>
	*/

	if (curPos >= (int) iStr.GetLength()) {		
		return 0;
	}

	unsigned char c = (unsigned char) iStr[curPos];
	if (c <= 31) return 0;
	else if (c == 127) return 0;
	else if (c == ' ') return 0;
	else if (In(c, g_strRfc822EmailAddressSpecials)) return 0;
	else {
		curPos++;
		return c;
	}
}

CString CEmailAddress::MPAtomSequence(const CString& iStr, int& curPos){
	
	CString Result;
	char c;
	do {
		c = MPAtom(iStr, curPos);
		if (c) Result+=c;
	} while(c);
	
	return Result;
}

int CEmailAddress::In(const char& What, const CString& Where){
	for (int i=0;i<(int) Where.GetLength();i++)
		if (Where[i] == What) return 1;
		return 0;
}

CString CEmailAddress::MPDomainRef(const CString& iStr, int& curPos){
	return MPAtomSequence(iStr, curPos);
}

char CEmailAddress::MPDtext(const CString& iStr, int& curPos){
	/*
	  dtext		=  <any CHAR excluding "[", 	; => may be folded
	  "]", "\" & CR, & including
	  linear-white-space>
	*/	

	if (curPos < (int) iStr.GetLength()) {
		char c = iStr[curPos];
		switch(c){
		case '[':
		case ']':
		case '\\':
		case CR:
			return 0;
		}
		char d;
		if ((d = MPLinearWhiteSpace(iStr, curPos))) return d;
		return c;
	} else return 0;
}

char CEmailAddress::MPLinearWhiteSpace(const CString& iStr, int& curPos){
	/*
	  linear-white-space =  1*([CRLF] LWSP-char) ; semantics = SPACE
	  ; CRLF => folding
	*/
	if (curPos < (int) iStr.GetLength()) {
		int workingPos = curPos;
		if ((iStr[workingPos] == 13)&&(iStr[workingPos+1] == 10)) workingPos+=2;
		char c = iStr[workingPos];
		if ((c==SPACE)||(c==HTAB)) {
			curPos = workingPos+1;
			return c;
		} else return 0;
	} else return 0;
}

CString CEmailAddress::MPDomainLiteral(const CString& iStr, int& curPos){
	/*
	  domain-literal =  "[" *(dtext / quoted-pair) "]"
	*/

	if (curPos < (int) iStr.GetLength()) {
		int workingPos = curPos;
		if (iStr[workingPos] == '[') {
			workingPos++;
			while (workingPos < (int) iStr.GetLength()) {
				if (MPDtext(iStr, workingPos)) workingPos++;
				else if (MPQuotedPair(iStr, workingPos)) workingPos++;
				else break;
			}
			if (iStr[workingPos] == ']') {
				CString Result;
                                iStr.Mid(curPos, workingPos - curPos, &Result);
				curPos = workingPos+1;
				return Result;
			} else return CString::EmptyCString;
		} else return CString::EmptyCString;
	} else return CString::EmptyCString;
}

char CEmailAddress::MPQuotedPair(const CString& iStr, int& curPos){

	if (curPos < (int) iStr.GetLength()) {
		if (iStr[curPos] == '\\') {
			curPos++;
			return iStr[curPos];
		} else return 0;
	} else return 0;
}
