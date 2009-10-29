/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:    

*/

#ifndef BASE_PMDF_EMAIL_HPP
#define BASE_PMDF_EMAIL_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <String/StringTable.hpp>
#include <Email/mimemessage.hpp>
#include <HashTable/VectorTable.hpp>

class CPmdfEmail : public CMimeMessage {
	property(CStringTable, Environ); 
	property(CString, RawBuffer);
	property(CVectorTable<CStringTable>, PmdfFields); // requested HTTP fields	
private:	
	bool ReadEnviron(void); // read the PMDF environement variables
	bool ReadFromStdin(void); // read the email on the standard input
public:
	bool Parse(CString&); // parse the read email
	CPmdfEmail(void);
	virtual ~CPmdfEmail(void);
	inline CString GetPmdfValue(const CString& Name) const { return m_PmdfFields.FindElement(Name).GetValue(Name); }	
	CString GetPmdfFrom(void) const;
};

#endif
