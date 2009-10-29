/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#ifndef BASE_HASHTABLE_HPP
#define BASE_HASHTABLE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>
#include <Tree/XmlTree.hpp>

template <class T>
class CHashTable : public CObject {	
    CVector<T> * m_pElements;
    CVector<CString> * m_pKeys;
    readonly_property(int, Size);
private:	
    int Hash(const CString&, int) const;
    void Grow(int NewSize = -1);
public:
    CHashTable(void);
    CHashTable(const CHashTable<T>&);
    CHashTable<T>& operator=(const CHashTable<T>&);
    virtual ~CHashTable();
    bool RemoveAt(const unsigned int);
    bool Remove(const CString&);
    bool Set(const CString&, const T&);
    CString GetKeyAt(const int) const;
    inline const T& operator[](const int Index) const { return GetElementAt(Index); }
    inline T& operator[](const int Index) { return GetElementAt(Index); }
    const T& GetElementAt(const int) const;
    T& GetElementAt(const int);
    int FindElementIndex(const CString&) const;
    bool FindAndCopy(const CString&, T&) const;
    const T * FindElement(const CString& Key) const;
    const T * SetElement(const CString& Key, const T& Value, bool * pAdded);
    void RemoveAll();
    ostream& operator<<(ostream&) const;
};

template <class T>
inline ostream& operator<<(ostream& Stream, const CHashTable<T>& Table) {
    return Table.operator<<(Stream);
}

#endif
