/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#ifndef BASE_LIST_CPP
#define BASE_LIST_CPP

#include "List.hpp"
 
template <class T>
CList<T>::CList(void) :  
  m_Head(NULL),
  m_Tail(NULL),
#ifdef BASE_DEBUG
  BASE_GUARD_INIT(m_PropGuard),
  BASE_GUARD_INIT(m_HeapGuard),
#endif
  m_Size(0)
{
  
}

template <class T>
CList<T>::CList(const CList& RefList) :
  m_Head(NULL),
  m_Tail(NULL),
#ifdef BASE_DEBUG
  BASE_GUARD_INIT(m_PropGuard),
  BASE_GUARD_INIT(m_HeapGuard),
#endif
  m_Size(0)
{
  
  operator=(RefList);
}

template <class T>
CList<T>& CList<T>::operator=(const CList& RefList) {       
	if (&RefList != this) {		
		RemoveAll();
		CListElement<T> * Current = RefList.m_Head;
		while (Current) {
			Add(Current->m_Element);			
			Current = Current->m_Next;
		}
	}
	return * this;
}

template <class T>
CList<T>::~CList(void) {
    RemoveAll();            
    BASE_GUARD_CHECK(m_HeapGuard);
    BASE_GUARD_CHECK(m_PropGuard);
}

template <class T>
void CList<T>::RemoveAll(void) {
    CListElement<T> * Current = m_Tail;
    CListElement<T> * Prev;
    while (Current) {
        Prev = Current->m_Prev;
        delete Current;
        Current = Prev;
    }
    m_Head = NULL;
    m_Tail = NULL;
    m_Size = 0;	
}

template <class T>
void CList<T>::Add(const T& Element) {	
    if (!m_Tail) {
        m_Tail = new CListElement<T>(Element);
        m_Head = m_Tail;
    } else {
        m_Tail->m_Next = new CListElement<T>(Element);
        m_Tail->m_Next->m_Prev = m_Tail;
        m_Tail = m_Tail->m_Next;
    }
    m_Size++;
}

template <class T>
bool CList<T>::operator<(const CList<T>& RefList) const {
	if (this == &RefList) return false;
	else if (m_Size < RefList.m_Size) return true;
	else if (m_Size > RefList.m_Size) return false;
	else {
		CListElement<T> * CurrentLeft = m_Head;
		CListElement<T> * CurrentRight = RefList.m_Head;
		while (CurrentLeft && CurrentRight) {
			if ((* CurrentLeft) < (* CurrentRight)) return true;
			CurrentLeft = CurrentLeft->m_Next;
			CurrentRight = CurrentRight->m_Next;
		}
		return false;
	}
}

template <class T>
bool CList<T>::operator>(const CList<T>& RefList) const {
	if (this == &RefList) return false;
	else if (m_Size > RefList.m_Size) return true;
	else if (m_Size < RefList.m_Size) return false;
	else {
		CListElement<T> * CurrentLeft = m_Head;
		CListElement<T> * CurrentRight = RefList.m_Head;
		while (CurrentLeft && CurrentRight) {
			if ((* CurrentLeft) > (* CurrentRight)) return true;
			CurrentLeft = CurrentLeft->m_Next;
			CurrentRight = CurrentRight->m_Next;
		}
		return false;
	}
}

template <class T>
bool CList<T>::operator<=(const CList<T>& RefList) const {
	if (this == &RefList) return true;
	else if (m_Size < RefList.m_Size) return true;
	else if (m_Size > RefList.m_Size) return false;
	else {
		CListElement<T> * CurrentLeft = m_Head;
		CListElement<T> * CurrentRight = RefList.m_Head;
		while (CurrentLeft && CurrentRight) {
			if ((* CurrentLeft) > (* CurrentRight)) return false;
			CurrentLeft = CurrentLeft->m_Next;
			CurrentRight = CurrentRight->m_Next;
		}
		return true;
	}
}

template <class T>
bool CList<T>::operator>=(const CList<T>& RefList) const {
	if (this == &RefList) return true;
	else if (m_Size > RefList.m_Size) return true;
	else if (m_Size < RefList.m_Size) return false;
	else {
		CListElement<T> * CurrentLeft = m_Head;
		CListElement<T> * CurrentRight = RefList.m_Head;
		while (CurrentLeft && CurrentRight) {
			if ((* CurrentLeft) < (* CurrentRight)) return false;
			CurrentLeft = CurrentLeft->m_Next;
			CurrentRight = CurrentRight->m_Next;
		}
		return true;
	}
}

template <class T>
bool CList<T>::operator!=(const CList<T>& RefList) const {
	if (this == &RefList) return false;
	else if (m_Size != RefList.m_Size) return true;
	else {
		CListElement<T> * CurrentLeft = m_Head;
		CListElement<T> * CurrentRight = RefList.m_Head;
		while (CurrentLeft && CurrentRight) {
			if ((* CurrentLeft) != (* CurrentRight)) return true;
			CurrentLeft = CurrentLeft->m_Next;
			CurrentRight = CurrentRight->m_Next;
		}
		return false;
	}
}

template <class T>
bool CList<T>::operator==(const CList<T>& RefList) const {
	if (this == &RefList) return true;
	else if (m_Size != RefList.m_Size) return false;
	else {
		CListElement<T> * CurrentLeft = m_Head;
		CListElement<T> * CurrentRight = RefList.m_Head;
		while (CurrentLeft && CurrentRight) {
			if ((* CurrentLeft) != (* CurrentRight)) return false;
			CurrentLeft = CurrentLeft->m_Next;
			CurrentRight = CurrentRight->m_Next;
		}
		return true;
	}
}

template <class T>
const T& CList<T>::operator[](const unsigned int Index) const {
	_S_DEBUG(assert(Index < m_Size));
	CListElement<T> * Current = m_Head;
	unsigned int Counter = 0;
	while (Counter != Index) {
		Counter++;
		Current = Current->m_Next;
	}
	return Current->m_Element;
}

template <class T>
T& CList<T>::operator[](const unsigned int Index) {
	_S_DEBUG(assert(Index < m_Size));
	CListElement<T> * Current = m_Head;
	unsigned int Counter = 0;
	while (Counter != Index) {
		Counter++;
		Current = Current->m_Next;
	}
	return Current->m_Element;
}

template <class T>
void CList<T>::SetAt(const unsigned int Index, const T& Element) {
	if (Index >= m_Size) Add(Element);
	else {
		CListElement<T> * Current = m_Head;
		unsigned int Counter = 0;
		while (Counter != Index) {
			Counter++;
			Current = Current->m_Next;
		}
		Current->m_Element = Element;
	}
}

template <class T>
int CList<T>::Find(const T& Element) const {
	CListElement<T> * Current = m_Head;
	int Counter = 0;
	while (Current) {
		if (Current->m_Element == Element) return Counter;
		Counter++;
		Current = Current->m_Next;
	}
	return -1;
}

template <class T>
void CList<T>::Add(const CList<T>& List) {       
	if (List.m_Size) {
		CListElement<T> * RightCurrent = List.m_Head;
		CListElement<T> * Current = new CListElement<T>(RightCurrent->m_Element);
		CListElement<T> * NewHead = Current;
		RightCurrent = RightCurrent->m_Next;
		while (RightCurrent) {
			Current->m_Next = new CListElement<T>(RightCurrent->m_Element);
			Current->m_Next->m_Prev = Current;
			Current = Current->m_Next;
			RightCurrent = RightCurrent->m_Next;
		}
		if (m_Tail) {
			NewHead->m_Prev = m_Tail;
			m_Tail->m_Next = NewHead;
			m_Tail = Current;
		} else {
			m_Head = NewHead;
			m_Tail = Current;
		}
		m_Size += List.m_Size;
	}
}

template <class T>
void CList<T>::Remove(const T& Element) {
    CListElement<T> * Current = m_Head;	
    while (Current) {
        if (Current->m_Element == Element) {
            if (Current->m_Prev)
                Current->m_Prev->m_Next = Current->m_Next;
            else m_Head = Current->m_Next;
            if (Current->m_Next)
                Current->m_Next->m_Prev = Current->m_Prev;
            else m_Tail = Current->m_Prev;
            CListElement<T> * Future = Current->m_Next;
            delete Current;
            m_Size--;
            Current = Future;
        } else Current = Current->m_Next;
    }
}

template <class T>
void CList<T>::RemoveAt(const unsigned int Index) {
	if (m_Size > Index) {		
		CListElement<T> * Current = m_Head;
		unsigned int Counter = 0;
		while (Current) {
			if (Counter == Index) {
				if (Current->m_Prev)
					Current->m_Prev->m_Next = Current->m_Next;
				else m_Head = Current->m_Next;
				if (Current->m_Next)
					Current->m_Next->m_Prev = Current->m_Prev;
				else m_Tail = Current->m_Prev;
				delete Current;
				m_Size--;
				return;
			} else {
				Current = Current->m_Next;
				Counter++;
			}
		}
	}
}

template <class T>
void CList<T>::InsertAt(const unsigned int Index, const T& Element) {
	if (Index >= m_Size) Add(Element);
	else {	       
		CListElement<T> * Current = m_Head;
		CListElement<T> * NewElement = new CListElement<T>(Element);
		unsigned int Counter = 0;
		while (Current && (Counter < Index)) {
			Current = Current->m_Next;
			Counter++;
		}
		if (Current) {
			NewElement->m_Prev = Current->m_Prev;
			NewElement->m_Next = Current;
			if (Current->m_Prev)
				Current->m_Prev->m_Next = NewElement;
			else m_Head = NewElement;
			Current->m_Prev = NewElement;
			m_Size++;
		} else Add(Element);
	}
}

#endif
