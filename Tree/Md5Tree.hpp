/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_MD5_TREE_HPP
#define BASE_MD5_TREE_HPP

#include <platform/include.hpp>
#include <String/String.hpp> 
#include <Tree/Tree.hpp>
#include <Vector/IntVector.hpp>
 
class CMd5Tree : private CTree<unsigned char> {
    protected_property(CVector<CObject *>, IndexVector);
public:
    CMd5Tree(void);
    virtual ~CMd5Tree(void);
public:
    CVector<unsigned char> Md5ToVector(const CString& Md5String) const;
    bool Add(const CString& Md5String, int Md5Index);
    inline bool Contains(const CVector<unsigned char>& Md5Vector) const { return (Find(Md5Vector) != NULL); }
    inline bool Contains(const CString& Md5String) const { return Contains(Md5ToVector(Md5String)); }
    inline int FindNodeIndex(CTreeElement<unsigned char> * Element) const;
    inline int FindNodeIndex(const CVector<unsigned char>& Md5Vector) const { return FindNodeIndex(Find(Md5Vector)); }
    inline int FindNodeIndex(const CString& Md5String) const { return FindNodeIndex(Md5ToVector(Md5String)); }
    CString GetAt(unsigned int) const;
    inline CString operator[](unsigned int Index) const { return GetAt(Index); }
    inline void SetDim(unsigned int NewSize) { m_IndexVector.SetDim(NewSize); }
    inline unsigned int GetSize(void) const { return m_IndexVector.GetSize(); }
    inline void RemoveAll(void);
    inline ostream& operator<<(ostream& Stream) const { return CTree<unsigned char>::operator<<(Stream); }
    friend inline ostream& operator<<(ostream&, const CMd5Tree&);
    int FindNodeIndexes(CTreeElement<unsigned char> * Element, CIntVector& Result, bool SameLevel = false) const;
    int FindNodeIndexes(const CVector<unsigned char>& Md5Vector, CIntVector& Result) const;
    int FindNodeIndexes(const CString& Md5String, CIntVector& Result) const { return FindNodeIndexes(Md5ToVector(Md5String), Result); }
};

inline ostream& operator<<(ostream& Stream, const CMd5Tree& Tree) {
    return Tree.operator<<(Stream);
}

inline int CMd5Tree::FindNodeIndex(CTreeElement<unsigned char> * Element) const {
    for (register int i=((int)m_IndexVector.GetSize())-1;i>=0;i--)
        if ((CTreeElement<unsigned char> *) m_IndexVector[i] == Element) return i;
        return -1;
}

inline void CMd5Tree::RemoveAll(void) { 
    m_IndexVector.RemoveAll(); 
    CTree<unsigned char>::RemoveAll(); 
}

#endif
