/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_CSTRING_THREADSAFE_TABLE_HPP
#define BASE_CSTRING_THREADSAFE_TABLE_HPP

#include <platform/platform.hpp>
#include <String/StringPair.hpp>
#include <String/StringTable.hpp>
#include <Vector/Vector.hpp>
#include <Mutex/RWMutex.hpp>
#include <Tree/XmlTree.hpp>

class CTSStringTable : public CStringTable {
    mutable_property(CRWMutex, RWMutex);
public:
    CTSStringTable(void);
    CTSStringTable(const CTSStringTable&);
    virtual ~CTSStringTable(void);
    virtual CTSStringTable& operator=(const CTSStringTable&);
    virtual CStringTable& operator=(const CStringTable&);
    virtual ostream& operator<<(ostream&) const;
    /* addition */
    virtual void Add(const CTSStringTable& StringTable);
    virtual void Add(const CString&, const CString&);
    virtual void Set(const CString&, const CString&);
    virtual void SetAt(const unsigned int, const CString&, const CString&);
    virtual void InsertAt(const unsigned int, const CString&, const CString&);
    /* retrieval */
    virtual CString GetValue(const CString&) const;
    virtual CString GetNameAt(const unsigned int) const;
    virtual CString GetValueAt(const unsigned int) const;
    virtual bool FindAndCopy(const CString& Name, CString& Target) const;
    virtual int FindName(const CString& Name) const;    
    /* removal */
    virtual void RemoveAll(void);
    virtual void RemoveAt(const unsigned int);
    virtual void Remove(const CString&);
    virtual CString Get(void) const;
};

#endif
