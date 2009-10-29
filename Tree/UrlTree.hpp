/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_URL_TREE_HPP
#define BASE_URL_TREE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Tree/Tree.hpp>
#include <Internet/Url.hpp>
#include <File/Progress.hpp>
#include <Vector/IntVector.hpp>

class CUrlTree : public CTree<CString> {    
	protected_property(CVector<CString>, UrlVector);
	protected_property(CVector<CObject *>, IndexVector);
	property(bool, Insens);
	mutable CProgress m_Progress;
public:
	CUrlTree(void);
	virtual ~CUrlTree(void);
public:
	CVector<CString> UrlToVector(const CUrl&) const;
	inline int Add(const CString& UrlString, bool * AddedExternal = NULL) { return Add(UrlToVector(UrlString), AddedExternal); }
    int Add(const CVector<CString>& UrlVector, bool * AddedExternal = NULL);
	int Add(const CUrl&, bool * = NULL);
	inline bool Contains(const CVector<CString>& UrlVector) const { return (Find(UrlVector) != NULL); }
    inline bool Contains(const CUrl& Url) const { return Contains(UrlToVector(Url)); }
    inline bool Contains(const CString& UrlString) const { return Contains(CUrl(UrlString)); }
    int FindNodeIndex(CTreeElement<CString> * Element) const;
    inline int FindNodeIndex(const CVector<CString>& UrlVector) const { return FindNodeIndex(Find(UrlVector)); }
    inline int FindNodeIndex(const CUrl& Url) const { return FindNodeIndex(UrlToVector(Url)); }
    int FindNodeIndexes(CTreeElement<CString> * Element, CIntVector& Result, bool SameLevel = false) const;
    int FindNodeIndexes(const CVector<CString>& UrlVector, CIntVector& Result) const;
    inline int FindNodeIndexes(const CUrl& Url, CIntVector& Result) const { return FindNodeIndexes(UrlToVector(Url), Result); }
    CString ResolveAt(unsigned int) const;
    inline CString GetAt(unsigned int Index) const { return m_UrlVector[Index]; }
    inline CString operator[](unsigned int Index) const { return m_UrlVector[Index]; }
    inline void SetDim(unsigned int NewSize) { m_IndexVector.SetDim(NewSize); m_UrlVector.SetDim(NewSize); }
    inline unsigned int GetSize(void) const { return m_IndexVector.GetSize(); }
    inline void RemoveAll(void) { m_IndexVector.RemoveAll(); m_UrlVector.RemoveAll(); CTree<CString>::RemoveAll(m_Head); }
    bool FastLoad(const CString& Filename, bool Verbose = true);
    bool Load(const CString& FileName, bool Verbose = true);
	bool Save(const CString& FileName, bool Verbose = true, int DataRows = 0) const;
	virtual inline bool Equal(const CString& Left, const CString& Right) const { return ((m_Insens)?Left.Same(Right):Left.Equal(Right)); }
	virtual inline bool Greater(const CString& Left, const CString& Right) const { return ((m_Insens)?Left.BiggerInsens(Right):Left.Bigger(Right)); }
	virtual inline bool Smaller(const CString& Left, const CString& Right) const { return ((m_Insens)?Left.SmallerInsens(Right):Left.BiggerInsens(Right)); }
	inline ostream& operator<<(ostream& Stream) const { return CTree<CString>::operator<<(Stream); }
	friend inline ostream& operator<<(ostream&, const CUrlTree&);
	bool SaveNode(FILE * Stream, CTreeElement<CString> * Element, int Level, bool Verbose, long& Total, int DataRows = 0) const;
	bool AddFast(const CString& Line, CVector<CObject *>& HorizontalVector);
};

inline ostream& operator<<(ostream& Stream, const CUrlTree& Tree) {
	return Tree.operator<<(Stream);
}


#endif
