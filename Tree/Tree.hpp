/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:
    
    16.09.1999: return type for CTreeElement::operator<>=! (gcc 2.95.1)

*/

#ifndef BASE_TREE_HPP
#define BASE_TREE_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <Vector/Vector.hpp>

template <class T>
class CTree;

template <class T>
class CTreeElement {
    friend class CTree<T>;
    readonly_property(T, Element);
    copy_property(void *, Data);
    copy_property(CTreeElement<T> *, Next);
    copy_property(CTreeElement<T> *, Prev);
    copy_property(CTreeElement<T> *, Child);
    copy_property(CTreeElement<T> *, LastChild);
    copy_property(CTreeElement<T> *, Parent);
private:
    inline void Init(void) { 
        m_Next = m_Prev = m_Child = m_Parent = m_LastChild = NULL; 
        m_Data = NULL;
    }
public:
    inline CTreeElement(void) { Init(); }
    inline CTreeElement(const T& Element) {
        Init(); 
        m_Element = Element; 
    }
    inline CTreeElement(const CTreeElement<T>& TreeElement) { 
        Init(); 
        m_Element = TreeElement.m_Element; 
        m_Data = TreeElement.m_Data;
    }
    inline virtual ~CTreeElement(void) { }
    inline bool operator<(const CTreeElement<T>& Other) const { return m_Element < Other.m_Element; }
    inline bool operator>(const CTreeElement<T>& Other) const { return m_Element > Other.m_Element; }
    inline bool operator<=(const CTreeElement<T>& Other) const { return m_Element <= Other.m_Element; }
    inline bool operator>=(const CTreeElement<T>& Other) const { return m_Element >= Other.m_Element; }
    inline bool operator!=(const CTreeElement<T>& Other) const { return m_Element != Other.m_Element; }
    inline bool operator==(const CTreeElement<T>& Other) const { return m_Element == Other.m_Element; }
    inline ostream& operator<<(ostream& Stream) const { Show(Stream); return Stream; }
    void Show(ostream& Stream, int Depth = 0) const;    
};

template <class T>
class CTree : public CObject {
    property(bool, Sorted);
    property(bool, Unique);
protected:
    copy_property(CTreeElement<T> *, Head);
    void RemoveAll(CTreeElement<T> *& Node);
    virtual inline bool Equal(const T& Left, const T& Right) const { return Left == Right; }
    virtual inline bool Greater(const T& Left, const T& Right) const { return Left > Right; }
    virtual inline bool Smaller(const T& Left, const T& Right) const { return Left < Right; }
public:
    CTree<T>(bool Sorted = false, bool Unique = false);
    virtual ~CTree(void);
    inline void RemoveAll(void) { RemoveAll(m_Head); }
    void Remove(CTreeElement<T> *& Node);
    CTreeElement<T> * AddChildFirst(CTreeElement<T> * Node, const T& Element);
    CTreeElement<T> * AddChildLast(CTreeElement<T> * Node, const T& Element);
    CTreeElement<T> * Add(CTreeElement<T> * Node, const T& Element, bool * = NULL);
    CTreeElement<T> * AddChild(CTreeElement<T> * Node, const T& Element, bool * = NULL);
    CTreeElement<T> * AddBefore(CTreeElement<T> * Node, const T& Element);
    CTreeElement<T> * AddAfter(CTreeElement<T> * Node, const T& Element);
    CTreeElement<T> * Find(const CVector<T>&) const;
    CTreeElement<T> * Add(const CVector<T>&, bool * = NULL);
    void MoveAsChildLast(CTree<T>& Source, CTreeElement<T> * Node, CTreeElement<T> * TargetNode);
    inline ostream& operator<<(ostream& Stream) const { if (m_Head) m_Head->Show(Stream); return Stream; }
};

template <class T>
inline ostream& operator<<(ostream& Stream, const CTree<T>& Tree) {
    return Tree.operator<<(Stream);
}

#endif
