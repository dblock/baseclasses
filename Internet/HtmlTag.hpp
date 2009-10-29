/*

    © Xo3 Digital Systems, Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_HTML_TAG_HPP
#define BASE_HTML_TAG_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <String/StringTable.hpp>

class CHtmlTag : public CObject {
	property(CString, Name);
	property(CStringTable, Parameters);
	property(CString, Free);
public:
	CHtmlTag(void);
	CHtmlTag(const CHtmlTag&);
	CHtmlTag(const CString&);
    CHtmlTag& operator=(const CHtmlTag& HtmlTag);
	virtual ~CHtmlTag(void);
	inline void Add(const CString& Name, const CString& Value) { m_Parameters.Add(Name, Value); }
	inline void Set(const CString& Name, const CString& Value) { m_Parameters.Set(Name, Value); }
    CString GetBuffer(void) const;
	ostream& operator<<(ostream& Stream) const;
	istream& operator>>(istream& Stream);
    friend ostream& operator<<(ostream&, const CHtmlTag&);
	friend istream& operator>>(istream&, CHtmlTag&);
};

inline ostream& operator<<(ostream& Stream, const CHtmlTag& Tag) {
	return Tag.operator<<(Stream);
}

inline istream& operator>>(istream& Stream, CHtmlTag& Tag) {
	return Tag.operator>>(Stream);
}

#endif
