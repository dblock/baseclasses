/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_CSTRING_TABLE_HPP
#define BASE_CSTRING_TABLE_HPP

#include <platform/platform.hpp>
#include <String/StringPair.hpp>
#include <Vector/Vector.hpp>
#include <Mutex/RWMutex.hpp>
#include <Tree/XmlTree.hpp>

class CStringTable : public CObject {
    property(CVector<CStringPair>, Table);    
public:
    CStringTable(void);
    CStringTable(const CStringTable&);
    virtual ~CStringTable(void);
    virtual CStringTable& operator=(const CStringTable&);
    virtual ostream& operator<<(ostream&) const;
    friend ostream& operator<<(ostream&, const CStringTable&);
    /* queries */
    virtual inline unsigned int GetSize(void) const { return m_Table.GetSize(); }
    /* addition */
    virtual void Add(const CStringTable& StringTable);
    virtual void Add(const CString&, const CString&);
    virtual inline void Add(const CStringPair& Pair) { Add(Pair.GetName(), Pair.GetValue()); }
    virtual void Set(const CString&, const CString&);
    virtual inline void Set(const CStringPair& Pair) { Set(Pair.GetName(), Pair.GetValue()); }
    virtual void SetAt(const unsigned int, const CString&, const CString&);
    virtual void InsertAt(const unsigned int, const CString&, const CString&);
    virtual inline void SetAt(const unsigned int Index, const CStringPair& Pair) { SetAt(Index, Pair.GetName(), Pair.GetValue()); }
    /* retrieval */
    virtual CString GetValue(const CString&) const;
    virtual CString GetNameAt(const unsigned int) const;
    virtual CString GetValueAt(const unsigned int) const;
    virtual bool Contains(const CString& Name) const { return (FindName(Name) != -1); }
    virtual bool FindAndCopy(const CString& Name, CString& Target) const;
    virtual int FindName(const CString& Name) const;
    virtual const CStringPair& operator[](const unsigned int) const;
    virtual CStringPair& operator[](const unsigned int);
    /* removal */
    virtual void RemoveAll(void);
    virtual void RemoveAt(const unsigned int);
    virtual void Remove(const CString&);
    /* mapping using the table */
    virtual inline CString& MapTermEach(CString& Term, int /* Dummy */ = 0) const { Term = GetValue(Term); return Term; }
    virtual CString Map(const CString& /* Format */) const;
    virtual CString& MapTerm(const CString& /* Format */, CString& /* Target */, int /* Dummy */ = 0) const;
    virtual void PopulateXmlNode(CXmlTree& Tree, CTreeElement< CXmlNode > * pXmlNode) const;
    virtual CString Get(void) const;
};

inline ostream& operator<<(ostream& Stream, const CStringTable& Table) {
    return Table.operator<<(Stream);
}

#endif
