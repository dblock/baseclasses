/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Iterator.hpp"

CIterator::CIterator(void) {
	m_Current = NULL;
}

CIterator::~CIterator(void) {
	
}

CIterator::CIterator(const CIterator& Iterator) {
	m_Current = NULL;
	operator=(Iterator);
}

CIterator& CIterator::operator=(const CIterator& Iterator) {
	m_Current = Iterator.m_Current;
	return * this;
}
