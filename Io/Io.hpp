/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_C_IO_HPP
#define BASE_C_IO_HPP

#include <platform/include.hpp>
#include <platform/messages.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>

class CIo : public CObject {
public:
	CIo(void);
	CIo(const CIo&);
	CIo& operator=(const CIo&);
	virtual ~CIo(void);	
};

#endif
