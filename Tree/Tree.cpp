/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Tree.hpp"

template <class T>
void CTreeElement<T>::Show(ostream& Stream, int Depth) const {
	for (register int i=0;i<Depth;i++) Stream << ' ';
	Stream << m_Element << endl;
	if (m_Child) m_Child->Show(Stream, Depth+1);
	if (m_Next) m_Next->Show(Stream, Depth);
}

template <class T>
CTree<T>::CTree(bool Sorted, bool Unique) : CObject() {
	m_Head = NULL;
	m_Sorted = Sorted;
	m_Unique = Unique;
}

template <class T>
CTree<T>::~CTree(void) {
	RemoveAll(m_Head);
}

template <class T>
void CTree<T>::RemoveAll(CTreeElement<T> *& Node) {
	if (!Node) return;
	if (Node->m_Child) { RemoveAll(Node->m_Child); Node->m_LastChild = NULL; }
	if (Node->m_Next) RemoveAll(Node->m_Next);
	delete Node;
	Node = NULL;
}

template <class T>
void CTree<T>::Remove(CTreeElement<T> *& Node) {
	if (!Node) return;
	if (Node->m_Child) { RemoveAll(Node->m_Child); Node->m_LastChild = NULL; }
	if (Node->m_Prev) Node->m_Prev->m_Next = Node->m_Next;
	else if (Node->m_Parent && (Node->m_Parent->m_Child == Node)) 
          Node->m_Parent->m_Child = Node->m_Next;
	delete Node;
	Node = NULL;
}

/* add child on top of list */
template <class T>
CTreeElement<T> * CTree<T>::AddChildFirst(CTreeElement<T> * Node, const T& Element) {
	CTreeElement<T> * NewElement = new CTreeElement<T>(Element);
	if (!Node) Node = m_Head;
	if (!Node) m_Head = NewElement;
	else {
		NewElement->m_Parent = Node;
		NewElement->m_Next = Node->m_Child;
		if (Node->m_Child) Node->m_Child->m_Prev = Node; else Node->m_LastChild = NewElement;
		Node->m_Child = NewElement;
	}
	return NewElement;
}

/* add child on bottom of list */
template <class T>
CTreeElement<T> * CTree<T>::AddChildLast(CTreeElement<T> * Node, const T& Element) {
	CTreeElement<T> * NewElement = new CTreeElement<T>(Element);
	if (!Node) Node = m_Head;
	if (!Node) m_Head = NewElement;
	else {
		NewElement->m_Parent = Node;
		if (!Node->m_Child) {
			Node->m_Child = NewElement;
			Node->m_LastChild = NewElement;
		} else {
			Node->m_LastChild->m_Next = NewElement;
			NewElement->m_Prev = Node->m_LastChild;
			Node->m_LastChild = NewElement;
		}
	}
	return NewElement;
}

/* add at the same level, uses m_Sorted and m_Unique */
template <class T>
CTreeElement<T> * CTree<T>::Add(CTreeElement<T> * Node, const T& Element, bool * Added) {
	if (Added) (* Added) = true;
	if (!Node) Node = m_Head;
	if (!Node) {
          m_Head = new CTreeElement<T>(Element);          
          return m_Head;
        }
	/* hack: try to add to parent's last element */
	if (Node->m_Parent) {
		if (m_Sorted) {
			if (Greater(Element, Node->m_Parent->m_LastChild->m_Element)) {
				return AddAfter(Node->m_Parent->m_LastChild, Element);
			} else if (m_Unique && (Equal(Node->m_Parent->m_LastChild->m_Element, Element))) {
				if (Added) (* Added) = false;
				return Node->m_Parent->m_LastChild;
			}
		}
		Node = Node->m_Parent->m_Child;
	} else Node = m_Head;
	/* classic insertion */
	while(1) {
		if (m_Unique && Equal(Node->m_Element, Element)) {
			if (Added) (* Added) = false;
			return Node;
		} else if (m_Sorted && (Greater(Node->m_Element, Element))) return AddBefore(Node, Element);
		if (Node->m_Next) Node = Node->m_Next;
		else break;
	}
	return AddAfter(Node, Element);
}

/* add a child, uses m_Sorted and m_Unique */
template <class T>
CTreeElement<T> * CTree<T>::AddChild(CTreeElement<T> * Node, const T& Element, bool * Added) {
	if (Added) (* Added) = true;
	if (!Node) Node = m_Head;
	if (!Node) {
          m_Head = new CTreeElement<T>(Element);          
          return m_Head;
        }
	/* hack: check the last element first */
	if (m_Sorted && Node->m_LastChild) {          
		if (Greater(Element, Node->m_LastChild->m_Element)) {
			return AddAfter(Node->m_LastChild, Element);
		} else if (m_Unique && (Equal(Node->m_LastChild->m_Element, Element))) {
			if (Added) (* Added) = false;
			return Node->m_LastChild;
		}
	}
	/* classic add to the tree */
	CTreeElement<T> * Current = Node->m_Child;
	if (!Current) return AddChildFirst(Node, Element);
	while(1) {
		if (m_Unique && Equal(Current->m_Element, Element)) {
			if (Added) (* Added) = false;
			return Current;
		} else if (m_Sorted && (Greater(Current->m_Element, Element))) {
			return AddBefore(Current, Element);
		}
		if (Current->m_Next) Current = Current->m_Next;
		else break;
	}
	return AddAfter(Current, Element);
}

/* insert before an element */
template <class T>
CTreeElement<T> * CTree<T>::AddBefore(CTreeElement<T> * Node, const T& Element) {
	if (!Node) Node = m_Head;
	if (!Node) {
          m_Head = new CTreeElement<T>(Element);          
          return m_Head;
        }
	CTreeElement<T> * NewElement = new CTreeElement<T>(Element);
	NewElement->m_Next = Node;
	NewElement->m_Prev = Node->m_Prev;
	NewElement->m_Parent = Node->m_Parent;
	if (Node->m_Prev) 
          Node->m_Prev->m_Next = NewElement;
	if (Node->m_Parent && (Node->m_Parent->m_Child == Node)) 
          Node->m_Parent->m_Child = NewElement;
	Node->m_Prev = NewElement;
	if (Node == m_Head) 
          m_Head = NewElement;
	return NewElement;
}

/* insert after an element */
template <class T>
CTreeElement<T> * CTree<T>::AddAfter(CTreeElement<T> * Node, const T& Element) {
	if (!Node) Node = m_Head;
	if (!Node) {
          m_Head = new CTreeElement<T>(Element);          
          return m_Head;
        }
	CTreeElement<T> * NewElement = new CTreeElement<T>(Element);
	NewElement->m_Next = Node->m_Next;
	if (Node->m_Next) 
          Node->m_Next->m_Prev = NewElement;
	if (Node->m_Parent && (Node->m_Parent->m_LastChild == Node)) 
          Node->m_Parent->m_LastChild = NewElement;
	NewElement->m_Parent = Node->m_Parent;
	NewElement->m_Prev = Node;
	Node->m_Next = NewElement;
	return NewElement;
}

template <class T>
CTreeElement<T> * CTree<T>::Find(const CVector<T>& EltVector) const {
	unsigned int CurrentIndex = 0;
	CTreeElement<T> * Current = m_Head;
	while (Current) {
		if (CurrentIndex >= EltVector.GetSize()) return NULL;
		if (Equal(Current->m_Element, EltVector[CurrentIndex])) {
			CurrentIndex++;
			if (CurrentIndex == EltVector.GetSize()) return Current;
			else Current = Current->m_Child;
		} else Current = Current->m_Next;
	}
	return NULL;
}

template <class T>
CTreeElement<T> * CTree<T>::Add(const CVector<T>& EltVector, bool * Added) {
	if (Added)
          (* Added) = false;
	if (!EltVector.GetSize()) 
          return NULL;
	CTreeElement<T> * Current = Add(m_Head, EltVector[0], Added);
	for (register int i=1;i<(int) EltVector.GetSize();i++)
		Current = AddChild(Current, EltVector[i], Added);
	return Current;
}

template <class T>
void CTree<T>::MoveAsChildLast(CTree<T>& Source, CTreeElement<T> * Node, CTreeElement<T> * TargetNode) {
	// break the parent link
	if (Source.m_Head == Node)
		Source.m_Head = NULL;
	if (Node->m_Parent && Node->m_Parent->m_Child == Node)
		Node->m_Parent->m_Child = Node->m_Prev ? Node->m_Prev : Node->m_Next;
	if (Node->m_Parent && Node->m_Parent->m_LastChild == Node)
		Node->m_Parent->m_LastChild = Node->m_Next ? Node->m_Next : Node->m_Prev;
	if (Node->m_Prev)
		Node->m_Prev->m_Next = Node->m_Next;
	// attach to the new tree
	if (!TargetNode) TargetNode = m_Head;
	if (!TargetNode) m_Head = Node;
	else {
		Node->m_Parent = TargetNode;
		if (!TargetNode->m_Child) {
			TargetNode->m_Child = Node;
			TargetNode->m_LastChild = Node;
		} else {
			TargetNode->m_LastChild->m_Next = Node;
			Node->m_Prev = TargetNode->m_LastChild;
			TargetNode->m_LastChild = Node;
		}
	}  
}

