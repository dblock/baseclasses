/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    _____________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Atomic.hpp"

CAtomic::CAtomic(void) : m_Value(0) {
	
}

CAtomic::CAtomic(long Value) : m_Value(Value) {
	
}

CAtomic::~CAtomic(void) {

}

