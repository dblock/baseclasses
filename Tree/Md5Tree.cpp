/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "Md5Tree.hpp"

CMd5Tree::CMd5Tree(void) {
    SetSorted(true);
    SetUnique(true);    
}

CMd5Tree::~CMd5Tree(void) {
    
}

CVector<unsigned char> CMd5Tree::Md5ToVector(const CString& Md5String) const {
    CVector<unsigned char> Vector;
    Vector.SetDim(Md5String.GetLength());
    for (register int i=0; i < (int) Md5String.GetLength(); i++)
        Vector += Md5String[i];
    return Vector;
}

bool CMd5Tree::Add(const CString& Md5String, int Md5Index) {
    bool bAdded;
    CTreeElement<unsigned char> * Node = CTree<unsigned char>::Add(Md5ToVector(Md5String), &bAdded);
    int IndexVectorSize = m_IndexVector.GetSize();
    if (IndexVectorSize <= Md5Index) {
        m_IndexVector.SetSize(Md5Index + 1);
        for (register int i=IndexVectorSize;i<Md5Index;i++)
            m_IndexVector[i] = NULL;
    }
    m_IndexVector[Md5Index] = (CObject *) Node;
    return bAdded;
}

CString CMd5Tree::GetAt(unsigned int Index) const {
    if (Index >= m_IndexVector.GetSize()) 
        return CString::EmptyCString;
    CTreeElement<unsigned char> * Node = (CTreeElement<unsigned char> *) m_IndexVector[Index];
    CString Result;
    while (Node) {
        Result.Insert(0, Node->GetElement());
        Node = Node->GetParent();
    }
    return Result;
}

int CMd5Tree::FindNodeIndexes(CTreeElement<unsigned char> * Element, CIntVector& Result, bool SameLevel) const {
    if (!Element) 
        return -1;
    
    if (Element->GetChild()) {
        FindNodeIndexes(Element->GetChild(), Result, true); 
    }
    
    if (SameLevel && Element->GetNext()) {
        FindNodeIndexes(Element->GetNext(), Result, true);
    }
    
    for (register int i=((int)m_IndexVector.GetSize())-1;i>=0;i--)
        if ((CTreeElement<unsigned char> *) m_IndexVector[i] == Element) {
            Result += i;
        }
        
        return Result.GetSize();
}

int CMd5Tree::FindNodeIndexes(const CVector<unsigned char>& Md5Vector, CIntVector& Result) const { 
    CTreeElement<unsigned char> * Element = Find(Md5Vector);
    if (!Element)
        return 0;
    return FindNodeIndexes(Element, Result); 
}
