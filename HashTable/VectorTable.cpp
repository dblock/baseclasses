/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "VectorTable.hpp"

template <class T>
void CVectorTable<T>::ReplacePrefix(const CString& Source, const CString& Target) {
	for (register int i=0;i<(int) m_Keys.GetSize();i++) {
		int Pos = m_Keys[i].Pos(Source);
		if (Pos != -1) {
			m_Keys[i].Delete(Pos, Source.GetLength());
			m_Keys[i].Insert(Pos, Target);
		}
	}
}

template <class T>
CVectorTable<T>::CVectorTable(void) {
	// _L_DEBUG(0, cout << "CVectorTable<T>::CVectorTable(void)" << endl);
}

template <class T>
CVectorTable<T>::~CVectorTable(void) {
	// _L_DEBUG(0, cout << "CVectorTable<T>::~CVectorTable(void)" << endl);
}

template <class T>
bool CVectorTable<T>::RemoveAt(const unsigned int Index){
	assert(Index < m_Keys.GetSize());
	m_Keys.RemoveAt(Index);
	m_Elements.RemoveAt(Index);
	return true;
}

template <class T>
bool CVectorTable<T>::Remove(const CString& Key){
	bool Result = false;
	for (register int i=(int)m_Keys.GetSize()-1;i>=0;i--) {
		if (Key.Same(m_Keys[i])) {
			m_Keys.RemoveAt(i);
			m_Elements.RemoveAt(i);
			Result = true;
		}
	}
	return Result;
}

template <class T>
void CVectorTable<T>::Add(const CString& Key, const T& Value) {
	m_Keys.Add(Key);
	m_Elements.Add(Value);
}

// returns true if value replaced
template <class T>
bool CVectorTable<T>::Set(const CString& Key, const T& Value){
	for (register int i=(int)m_Keys.GetSize()-1;i>=0;i--) {
		if (Key.Same(m_Keys[i])) {
			m_Elements[i] = Value;
			return true;
		}
	}
	Add(Key, Value);
	return false;
}

template <class T>
const CString& CVectorTable<T>::GetKeyAt(const int Index) const {
	return m_Keys[Index];
}

template <class T>
const T& CVectorTable<T>::GetElementAt(const int Index) const {
	return m_Elements[Index];
}

template <class T>
CVectorTable<T>::CVectorTable(const CVectorTable<T>& Source){
	operator=(Source);
}

template <class T>
CVectorTable<T>& CVectorTable<T>::operator=(const CVectorTable<T>& Source){
	if (this != &Source) {
		m_Keys = Source.m_Keys;
		m_Elements = Source.m_Elements;
	}
	return (* this);
}

template <class T>
bool CVectorTable<T>::FindAndCopy(const CString& Key, T& Target) const {
	for (register int i=(int)m_Keys.GetSize()-1;i>=0;i--) {
		if (Key.Same(m_Keys[i])) {
			Target = m_Elements[i];
			return true;
		}
	}
	return false;
}

template <class T>
int CVectorTable<T>::FindElementIndex(const CString& Key) const {
	for (register int i=(int)m_Keys.GetSize()-1;i>=0;i--) {
		if (Key.Same(m_Keys[i])) {
			return i;
		}
	}
	return -1;
}

template <class T>
T CVectorTable<T>::FindElement(const CString& Key) const {
	for (register int i=(int)m_Keys.GetSize()-1;i>=0;i--) {
		if (Key.Same(m_Keys[i])) {
			return m_Elements[i];
		}
	}
	T Tmp;
	return Tmp;
}

template <class T>
void CVectorTable<T>::RemoveAll()	{
	m_Elements.RemoveAll();
	m_Keys.RemoveAll();
}

template <class T>
ostream& CVectorTable<T>::operator<<(ostream& Stream) const {
	for (register int i=0;i<(int)m_Elements.GetSize();i++) {
		Stream << i << ": " << m_Keys[i] << "=" << m_Elements[i] << endl;
	}
	return Stream;
}


