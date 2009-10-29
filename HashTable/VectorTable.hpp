/*
	© Vestris Inc., Geneva Switzerland
	http://www.vestris.com, 1998, All Rights Reserved
	__________________________________________________

	written by Daniel Doubrovkine - dblock@vestris.com

	*/

#ifndef BASE_VECTORTABLE_HPP
#define BASE_VECTORTABLE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>

template <class T>
class CVectorTable : public CObject {
    private_property(CVector<T>, Elements);
    private_property(CVector<CString>, Keys);
public:
    void ReplacePrefix(const CString& Source, const CString& Target);
    CVectorTable(void);
    CVectorTable(const CVectorTable<T>&);
    CVectorTable<T>& operator=(const CVectorTable<T>&);
    virtual ~CVectorTable();
    bool RemoveAt(const unsigned int);
    bool Remove(const CString&);
    bool Set(const CString&, const T&);
    void Add(const CString&, const T&);
    const CString& GetKeyAt(const int) const;
    inline const T& operator[](const int Index) const { return GetElementAt(Index); }
    const T& GetElementAt(const int) const;
    int FindElementIndex(const CString&) const;
    T FindElement(const CString& Key) const;
    bool FindAndCopy(const CString& Key, T& Target) const;
    void RemoveAll();
    ostream& operator<<(ostream&) const;
    inline unsigned int GetSize(void) const { return m_Keys.GetSize(); }
};

template <class T>
inline ostream& operator<<(ostream& Stream, const CVectorTable<T>& Table) {
    return Table.operator<<(Stream);
}

#endif
