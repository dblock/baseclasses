/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Io.hpp"

CIo::CIo(void) : CObject() {
}

CIo::CIo(const CIo& Io) : CObject(Io) {
	assert(0);
}

CIo& CIo::operator=(const CIo&) {
	assert(0);
	return * this;
}

CIo::~CIo(void) {

}
