/*
	© Vestris Inc., Geneva, Switzerland
	http://www.vestris.com, 1994-1999 All Rights Reserved
	______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

	*/

#ifndef BASE_CSTRING_PAIR_HPP
#define BASE_CSTRING_PAIR_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

class CStringPair : public CObject {
	property(CString, Name);
	property(CString, Value);
public:
	CStringPair(void);
	CStringPair(const CStringPair&);
	CStringPair(const CString&, const CString&);
	CStringPair& operator=(const CStringPair&);
	virtual ~CStringPair(void);
	friend ostream& operator<<(ostream& Stream, const CStringPair& Pair);
	inline ostream& operator<<(ostream& Stream) const;
};

inline ostream& CStringPair::operator<<(ostream& Stream) const { Stream << m_Name << "=" << m_Value; return Stream; }
inline ostream& operator<<(ostream& Stream, const CStringPair& Pair) { return Pair.operator<<(Stream); }

#endif
