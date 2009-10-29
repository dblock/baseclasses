/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "IntVecIter.hpp"

CIntVectorIterator::CIntVectorIterator(void) {
	m_IntVector = NULL;
	ReleaseCurrent();
}

CIntVectorIterator::~CIntVectorIterator(void) {
	ReleaseCurrent();
}

CIntVectorIterator::CIntVectorIterator(const CIntVector& IntVector) {
	m_IntVector = (CIntVector *)&IntVector;
	ReleaseCurrent();
}

CIntVectorIterator& CIntVectorIterator::operator=(const CIntVectorIterator& Iterator) {
	if (m_IntVector == Iterator.m_IntVector) return * this;
	ReleaseCurrent();
	m_IntVector = Iterator.m_IntVector;
	return *this;
}

CIntVectorIterator::CIntVectorIterator(const CIntVectorIterator& Iterator) {
	m_IntVector = NULL;
	ReleaseCurrent();
	operator=(Iterator);
}

void CIntVectorIterator::ReleaseCurrent(void) {
	if (m_IntVector && m_IntVector->GetSize()) {
		m_Current = (int *) m_IntVector->GetCArray();
	} else {
		m_Current = NULL;
	}
	m_Disp = 0;
}

void CIntVectorIterator::Rewind(void) const {
	if (!m_IntVector) return;
	m_Current = (int *) m_IntVector->GetCArray();
	m_Disp = 0;
}

void CIntVectorIterator::Fwind(void) const {
	if (!m_IntVector) return;
	const int * LBound = m_IntVector->GetCArray();
	m_Current = (int *) LBound + m_IntVector->GetCArrayMemoryFill();
	m_Current--; // decrement to the last element
	/*  check if it's preceeded by a counter, that's almost like DecrementBound */
	if (m_Current - 1 > LBound) {
		if ((*(m_Current-1))&CIntVector::HByte) {
			m_Current--;
			m_Disp = ((* m_Current)&CIntVector::XByte)-1;
		} else m_Disp = 0;
	}
}
