/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_EXCEPTION_HPP
#define BASE_EXCEPTION_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

class CException {
	property(bool, AutoDelete);
	property(CString, Error);
public:
	inline bool Delete(void);
	inline CException(bool = true);
};

inline CException::CException(bool AutoDelete) {
	m_AutoDelete = AutoDelete;
}

inline bool CException::Delete(void) {
	if (m_AutoDelete) {
		delete this;
		return true;
	} else return false;
}

#endif
