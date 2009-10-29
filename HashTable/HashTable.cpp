/*
    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    10.09.1999: fixed operator=(), bug in copying elements (m_Size vs. m_Count)

*/

#include <baseclasses.hpp>

#include "HashTable.hpp"

template <class T>
CHashTable<T>::CHashTable(void) {	
	m_Size = 0;	
    m_pElements = NULL;
    m_pKeys = NULL;
}

template <class T>
CHashTable<T>::~CHashTable(void) {
	if (m_pElements)
        delete m_pElements;
    if (m_pKeys)
        delete m_pKeys;
}

template <class T>
bool CHashTable<T>::RemoveAt(const unsigned int Index){    
    if (m_pKeys && m_pElements) {
        unsigned int Counter = 0;
        for (register int i=0;i<(int)m_pKeys->GetSize();i++) {
            if ((* m_pKeys)[i].GetLength()) {
                if (Counter == Index) {
                    m_Size--;
                    (* m_pKeys)[i].Empty();
                    return true;
                } else Counter++;
            }
        }
    }
	return false;
}

template <class T>
bool CHashTable<T>::Remove(const CString& Key){  
    if (m_pKeys && m_pElements) {
        if (Key.GetLength()) {
            int Counter = 0;
            while(Counter < m_Size) {
                int HashValue = Hash(Key, Counter);
                if ((* m_pKeys)[HashValue] == Key) {
                    m_Size--;
                    (* m_pKeys)[HashValue].Empty();
                    return true;
                } else Counter++;
            }
        }
    }
	return false;
}

template <class T>
const T * CHashTable<T>::SetElement(const CString& Key, const T& Value, bool * pAdded) {
  
    if (m_pKeys && m_pElements) {
        
        if (pAdded)
            * pAdded = false;
        
        if (!Key.GetLength())
            return NULL;
        
        int Counter = 0;
        while(Counter < (int) m_pKeys->GetSize()) {
            int HashValue = Hash(Key, Counter);    
            if (!((* m_pKeys)[HashValue].GetLength())) {      
                (* m_pKeys)[HashValue] = Key;
                (* m_pElements)[HashValue] = Value;
                m_Size++;
                if (* pAdded)
                    * pAdded = true;
                return & (* m_pElements)[HashValue];
            } else if ((* m_pKeys)[HashValue] == Key) {
                (* m_pElements)[HashValue] = Value;            
                return & (* m_pElements)[HashValue];
            }
            Counter++;
        }
    }

    Grow();
    return SetElement(Key, Value, pAdded);  
}
 
// returns true if value replaced
template <class T>
bool CHashTable<T>::Set(const CString& Key, const T& Value){
    bool bResult = false;
    SetElement(Key, Value, & bResult);
    return bResult;
}

template <class T>
CString CHashTable<T>::GetKeyAt(const int Index) const {
    if (m_pKeys && m_pElements) {        
        int Counter = 0;
        for (register int i=0;i<(int) m_pKeys->GetSize();i++) {
            if ((* m_pKeys)[i].GetLength()) {
                if (Counter == Index) 
                    return (* m_pKeys)[i];
                Counter++;
            }
        }
    }

	return CString::EmptyCString;
}

template <class T>
const T& CHashTable<T>::GetElementAt(const int Index) const {
    if (m_pKeys && m_pElements) {
        int Counter = 0;
        for (register int i=0;i<(int)m_pKeys->GetSize();i++) {
            if ((* m_pKeys)[i].GetLength()) {
                if (Counter == Index) 
                    return (* m_pElements)[i];
                Counter++;
            }
        }
    }
	assert(0);
	return (* m_pElements)[0];
}

template <class T>
T& CHashTable<T>::GetElementAt(const int Index) {
    if (m_pKeys && m_pElements) {
        int Counter = 0;
        for (register int i=0;i<(int)m_pKeys->GetSize();i++) {
            if ((* m_pKeys)[i].GetLength()) {
                if (Counter == Index) 
                    return (* m_pElements)[i];
                Counter++;
            }
        }
    }
	assert(0);
	return (* m_pElements)[0];
}

template <class T>
CHashTable<T>::CHashTable(const CHashTable<T>& Source){
	m_Size = 0;
    m_pElements = NULL;
    m_pKeys = NULL;
    operator=(Source);
}

template <class T>
CHashTable<T>& CHashTable<T>::operator=(const CHashTable<T>& Source){
	if (& Source == this) 
        return * this;	
	    
    if (Source.m_pKeys && Source.m_pElements) {
        if (! m_pKeys)
            m_pKeys = new CVector<CString>;        
        if (! m_pElements)
            m_pElements = new CVector<T>;
        (* m_pKeys) = (* Source.m_pKeys);
        (* m_pElements) = (* Source.m_pElements);
    }

    m_Size = Source.m_Size;

	return * this;
}

template <class T>
bool CHashTable<T>::FindAndCopy(const CString& Key, T& Target) const {	
    if (m_pKeys && m_pElements) {
        int Index = FindElementIndex(Key);
        if (Index != -1) {
            Target = (* m_pElements)[Index];
            return true;
        } 
    }
    return false;
}

template <class T>
int CHashTable<T>::FindElementIndex(const CString& Key) const {	
    if (m_pKeys && m_pElements) {
        if (Key.GetLength()) {
            int Counter = 0;
            while(Counter < m_Size) {
                int HashValue = Hash(Key, Counter);
                if ((* m_pKeys)[HashValue] == Key) {			
                    return HashValue;
                } else if ((* m_pKeys)[HashValue].GetLength() == 0) {
                    break;
                }
                
                Counter++;
            }
        }
    }

	return -1;
}

template <class T>
const T * CHashTable<T>::FindElement(const CString& Key) const {
    if (m_pKeys && m_pElements) {
        int Index = FindElementIndex(Key);
        if (Index != -1) 
            return &(* m_pElements)[Index];
    }
	return NULL;
}

template <class T>
int CHashTable<T>::Hash(const CString& Key, int Counter) const {	
	unsigned long KeyVal = 0;
	unsigned long K2;	
	if (Key.GetLength() < 4) memcpy((char *) &KeyVal, (const char *) Key.GetBuffer(), Key.GetLength());
	if (Key.GetLength() >= 4) { memcpy(&KeyVal, (const char *) Key.GetBuffer(), 4); }
	if (Key.GetLength() >= 8) { memcpy(&K2, (const char *) Key.GetBuffer()+4, 4); KeyVal ^= K2; }
	if (Key.GetLength() >= 12) { memcpy(&K2, (const char *) Key.GetBuffer()+8, 4); KeyVal ^= K2; }
	KeyVal = abs(KeyVal);
	double v1 = KeyVal * 0.6180339887;  // (sqrt5-1)/2, Knuth
	double hv1 = (v1 - (int) v1);
	int h1 = (int) (m_pKeys->GetSize() * hv1);
	double v2 = KeyVal * 0.87380339887;  // double hash, me
	double hv2 = (v2 - (int) v2);
	int h2 = ((int) (m_pKeys->GetSize() * hv2)) * 2 + 1;
	int retval = (h1 + Counter*h2) % m_pKeys->GetSize();	
	return retval;	  
}

template <class T>
void CHashTable<T>::Grow(int NewSize) {	
    
	if (NewSize == -1)
        NewSize = m_pKeys ? m_pKeys->GetSize() + (((int)(m_pKeys->GetSize() / 5)) + 1) : 20;
    
	CVector<CString> * pKeys = m_pKeys;
	CVector<T> * pElements = m_pElements;

    m_Size = 0;
        
    m_pKeys = new CVector<CString>(NewSize, true);
    m_pElements = new CVector<T>(NewSize, true);

    if (pKeys && pElements) {
        for (register int i=0;i<(int) pKeys->GetSize();i++) {
            if ((* pKeys)[i].GetLength()) {
                Set((* pKeys)[i], (* pElements)[i]);
            }
        }
        
        delete pKeys;
        delete pElements;	
    }
}

template <class T>
void CHashTable<T>::RemoveAll()	{	
	m_Size = 0;
	if (m_pElements)
        m_pElements->RemoveAll();
	if (m_pKeys)
        m_pKeys->RemoveAll();	
}

template <class T>
ostream& CHashTable<T>::operator<<(ostream& Stream) const {
    if (m_pKeys && m_pElements) {
        for (register int i=0;i<(int)m_pElements->GetSize();i++) {
            if ((* m_pKeys)[i].GetLength())
                Stream << i << ": " << (* m_pKeys)[i] << "=" << (* m_pElements)[i] << endl;
            else Stream << i << ": empty" << endl;
        }
    }
	return Stream;
}


