/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

  This is an iterator for the compressed vectors.
  Target arrays get their C counter incremented.
  You can iterate through the whole thing,
	_______________________________________________

	CIntVectorIterator Iterator(const CIntVector&);
	while (* Iterator)
		cout << Iterator++;
	_______________________________________________

	CIntVectorIterator Iterator(const CIntVector&);
	Iterator.Forward();
	while (* Iterator)
		cout << Iterator--;
	_______________________________________________

	CIntVectorIterator Iterator(const CIntVector&);
	Iterator.Forward();
	while (* Iterator) {
		cout << (int) Iterator;
		Iterator--;
	}

*/

#ifndef BASE_INT_VECTOR_ITERATOR
#define BASE_INT_VECTOR_ITERATOR

#include <platform/include.hpp>
#include "IntVector.hpp"

class CIntVectorIterator : public CObject {
private:
	mutable int m_Disp; // displacement for intervals
	mutable int * m_Current; // current pointer insude the compressed structure
	copy_property(CIntVector *, IntVector); // vector that is being iterated
	CIntVectorIterator(void); // should never be used without object
public:
	CIntVectorIterator(const CIntVector& /* IntVector */);
	CIntVectorIterator(const CIntVectorIterator& /* Iterator */);
	CIntVectorIterator& operator=(const CIntVectorIterator& /* Iterator */);
	virtual ~CIntVectorIterator(void);
private:
	void ReleaseCurrent(void);
	inline void DecrementBound(void) const;
public:
	inline int GetCurrent(void) const; // return the current element
	inline void Increment(void) const; // increment to the next elt
	inline void Decrement(void) const; // decrement to the prev elt
	inline int operator++(int /* PostFixDummy */) const { int Current = GetCurrent(); Increment(); return Current; }
	inline int operator--(int /* PostFixDummy */) const { int Current = GetCurrent(); Decrement(); return Current; }
	inline int operator++(void) const { Increment(); return GetCurrent(); }
	inline int operator--(void) const { Decrement(); return GetCurrent(); }
	inline bool operator *(void) const; // test for inside bounds * Iterator
	void Fwind(void) const; // fwind the iterator for --
	void Rewind(void) const; // rewind the iterator to it's first element
	inline operator int() const { return GetCurrent(); }
};

inline int CIntVectorIterator::GetCurrent(void) const {
	if (!m_Current) return -1;
	if ((* m_Current)&CIntVector::HByte) {
		return ((* (m_Current+1)) + m_Disp);
	} else return (* m_Current);
}

inline void CIntVectorIterator::Increment(void) const {
	if (!m_Current) return;
	if ((* m_Current)&CIntVector::HByte) {
		int iEltCount = ((* m_Current)&CIntVector::XByte);
		m_Disp++;
		if (m_Disp >= iEltCount) {
			m_Current+=2;
			m_Disp=0;
		}
	} else {
		m_Current++;
	}
}

inline void CIntVectorIterator::DecrementBound(void) const {
	/* check whether the sized interval requires collapsing to it's counter */
	const int * LBound = m_IntVector->GetCArray();
	if (m_Current > LBound) {
		if ((* (m_Current - 1))&CIntVector::HByte) {
			m_Current--;
			m_Disp=((* m_Current)&CIntVector::XByte)-1;
		}
	}
}

inline void CIntVectorIterator::Decrement(void) const {
	if (!m_Current) return;
	if ((* m_Current)&CIntVector::HByte) {
		m_Disp--;
		if (m_Disp < 0) {
			m_Current--;
			m_Disp = 0;
			DecrementBound();
		}
	} else {
		m_Current--;
		DecrementBound();
	}
}

inline bool CIntVectorIterator::operator *(void) const {
	if (!m_IntVector) return false;
	/* have we decremented out of the left bound */
	const int * LBound = m_IntVector->GetCArray();
	if (m_Current < LBound) return false;
	/* have we incremented over the right bound */
	const int * RBound = LBound + m_IntVector->GetCArrayMemoryFill();
	if (m_Current >= RBound) return false;
	return true;
}

#endif
