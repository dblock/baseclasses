/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:
    
    13.09.1999: GetFirst() / GetNext() use CIterator
    13.09.1999: added GetLast() / GetPrev()

*/

#ifndef BASE_LIST_HPP
#define BASE_LIST_HPP

#include <platform/include.hpp>
#include <List/Iterator.hpp>


template<class T>
class CList;

template<class T>
class CListElement {
    friend class CList<T>;
    readonly_property(T, Element);
    BASE_GUARD(m_PropGuard);
    copy_property(CListElement<T> *, Next);
    copy_property(CListElement<T> *, Prev);
    BASE_GUARD(m_HeapGuard);
private:
    inline void Init(void) { m_Next = m_Prev = NULL; }
public:
    inline CListElement(void)
#ifdef BASE_DEBUG
        : BASE_GUARD_INIT(m_PropGuard)
        , BASE_GUARD_INIT(m_HeapGuard)
#endif
    {
        Init(); 
    }
    inline CListElement(const T& Element)
#ifdef BASE_DEBUG
        : BASE_GUARD_INIT(m_PropGuard)
        , BASE_GUARD_INIT(m_HeapGuard)
#endif
    { 
        Init();
        m_Element = Element; 
    }
    inline virtual ~CListElement(void) {            
        BASE_GUARD_CHECK(m_HeapGuard);
        BASE_GUARD_CHECK(m_PropGuard);
    }
    inline bool operator<(const CListElement& Other) const {
        return m_Element < Other.m_Element; 
    }
    inline bool operator>(const CListElement& Other) const { 
        return m_Element > Other.m_Element; 
    }
    inline bool operator<=(const CListElement& Other) const { 
        return m_Element <= Other.m_Element; 
    }
    inline bool operator>=(const CListElement& Other) const { 
        return m_Element >= Other.m_Element; 
    }
    inline bool operator!=(const CListElement& Other) const { 
        return m_Element != Other.m_Element; 
    }
    inline bool operator==(const CListElement& Other) const { 
        return m_Element == Other.m_Element; 
    }
};

template <class T>
class CList : public CObject {
    
    private_copy_property(CListElement<T> *, Head);
    private_copy_property(CListElement<T> *, Tail);
    BASE_GUARD(m_PropGuard);
    readonly_property(unsigned int, Size);
    BASE_GUARD(m_HeapGuard);
    public:
        CList<T>(void);
        CList<T>(const CList&);
        CList<T>& operator=(const CList&);
        virtual ~CList(void);
        /* addition, concatenation */
        inline void operator+=(const T& Elt) { Add(Elt); }
        inline void operator+=(const CList<T>& List) { Add(List); }
        void Add(const T&);
        void Add(const CList<T>&);
        void Remove(const T&);
        void RemoveAt(const unsigned int);
        void InsertAt(const unsigned int, const T&);
        /* removal */
        void RemoveAll(void);
        /* streaming */
        inline ostream& operator<<(ostream& Stream) const;
        inline istream& operator>>(istream& Stream);
        /* modification, access */        
        void SetAt(const unsigned int Index, const T& Element);        
        const T& operator[](const unsigned int Index) const;
        T& operator[](const unsigned int Index);
        int Find(const T& Element) const;
        /* comparison */
        bool operator<(const CList<T>& List) const;
        bool operator>(const CList<T>& List) const;
        bool operator<=(const CList<T>& List) const;
        bool operator>=(const CList<T>& List) const;
        bool operator==(const CList<T>& List) const;
        bool operator!=(const CList<T>& List) const;
        /* next/previous */
        inline const T * GetFirst(CIterator&) const;
        inline const T * GetNext(CIterator&) const;
        inline const T * GetLast(CIterator&) const;
        inline const T * GetPrev(CIterator&) const;            
};

template <class T>
inline const T* CList<T>::GetFirst(CIterator& Iterator) const {
    Iterator.SetCurrent((void *) m_Head);
    if (m_Head) return &(m_Head->m_Element);
    else return NULL;
}

template <class T>
inline const T* CList<T>::GetLast(CIterator& Iterator) const {
    Iterator.SetCurrent((void *) m_Tail);
    if (m_Tail) return &(m_Tail->m_Element);
    else return NULL;
}

template <class T>
inline const T* CList<T>::GetNext(CIterator& Iterator) const {
    if (Iterator.GetCurrent()) {
        Iterator.SetCurrent((void *)((CListElement<T> *)Iterator.GetCurrent())->GetNext());
        if (Iterator.GetCurrent()) return &(((CListElement<T> *) Iterator.GetCurrent())->m_Element);
        else return NULL;
    } else return NULL;
}

template <class T>
inline const T* CList<T>::GetPrev(CIterator& Iterator) const {
    if (Iterator.GetCurrent()) {
        Iterator.SetCurrent((void *)((CListElement<T> *)Iterator.GetCurrent())->GetPrev());
        if (Iterator.GetCurrent()) return &(((CListElement<T> *) Iterator.GetCurrent())->m_Element);
        else return NULL;
    } else return NULL;
}

template <class T>
inline ostream& CList<T>::operator<<(ostream& Stream) const {
    CListElement<T> * Current = m_Head;
    while (Current) {
        Stream << Current->GetElement() << endl;
        Current = Current->GetNext();
    }
    return Stream;
}

template <class T>
inline ostream& operator<<(ostream &Stream, const CList<T>& List) {
    return List.operator<<(Stream);
}

template <class T>
inline istream& CList<T>::operator>>(istream& Stream) {
    CListElement<T> * Current = m_Head;
    while (Current) {
        //Stream >> (Current->m_Element);
        Current = Current->GetNext();
    }
    return Stream;
}

#endif
