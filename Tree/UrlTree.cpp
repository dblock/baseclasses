/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "UrlTree.hpp"
#include <File/LocalFile.hpp>
#include <String/GStrings.hpp>
#include <File/MMapFile.hpp>

CUrlTree::CUrlTree(void) : m_Progress(10, false) {
    SetSorted(true);
    SetUnique(true);
    m_Insens = false;
}

CUrlTree::~CUrlTree(void) {
    
}

CVector<CString> CUrlTree::UrlToVector(const CUrl& Url) const {
    CVector<CString> Vector;
    
    CString MidString;
    
    MidString += Url.GetScheme();
    MidString += ":/";
    
#ifdef _UNIX
    if (Url.GetScheme().Same(g_strProto_FILE)) {
        MidString += "/";
    }       
#endif
    
    Vector += MidString;
           
    MidString = Url.GetHost();
    
    if (MidString.GetLength() && Url.GetPortValue() != 80) {
        MidString += ":";
        MidString += Url.GetPort();
    }
    
    if (MidString.GetLength()) {
        Vector += MidString;
    }
    
    int HostVectorSize = Vector.GetSize();
    CVector<CString> Vector2;
    CString::StrToVector(Url.GetUrlPath(), '/', &Vector2);
    
    Vector += Vector2;
    
    if (((int) Vector.GetSize() > HostVectorSize) && (!Vector[HostVectorSize].GetLength())) 
        Vector.RemoveAt(HostVectorSize);
    
    return Vector;
}

int CUrlTree::Add(const CUrl& Url, bool * AddedExternal) {
    // _L_DEBUG(3, cout << "CUrlTree::Add() - " << Url.GetBrute() << endl);
    bool Added = false;
    CTreeElement<CString> * Node = CTree<CString>::Add(UrlToVector(Url), &Added);
    int Result = -1;
    if (Added || ((Result = FindNodeIndex(Node)) == -1)) {
        if (AddedExternal) 
            (* AddedExternal) = true;
        m_IndexVector.Add((CObject *)Node);
        Result = ((int) m_IndexVector.GetSize()) - 1;
        m_UrlVector.Add(ResolveAt(Result));
        // set the index
        Node->SetData((void *) Result);
    } else {
        if (AddedExternal) 
            (* AddedExternal) = false;        
    }
    // _L_DEBUG(3, cout << "CUrlTree::Add() (result) " << endl << (* this) << endl);
    return Result;
}

int CUrlTree::Add(const CVector<CString>& UrlVector, bool * AddedExternal) {
    // _L_DEBUG(3, cout << "CUrlTree::Add() - " << UrlVector << endl);
    bool Added = false;
    CTreeElement<CString> * Node = CTree<CString>::Add(UrlVector, &Added);
    int Result = -1;
    if (Added || ((Result = FindNodeIndex(Node)) == -1)) {
        if (AddedExternal)
            (* AddedExternal) = true;
        m_IndexVector.Add((CObject *)Node);
        Result = ((int) m_IndexVector.GetSize()) - 1;
        m_UrlVector.Add(ResolveAt(Result));
        // set the index
        Node->SetData((void *) Result);
    } else {
        if (AddedExternal) 
            (* AddedExternal) = false;        
    }
    // _L_DEBUG(3, cout << "CUrlTree::Add() (result) " << endl << (* this) << endl);    
    return Result;
}

CString CUrlTree::ResolveAt(unsigned int Index) const {    
    if (Index >= m_IndexVector.GetSize()) 
        return CString::EmptyCString;
    CTreeElement<CString> * Node = (CTreeElement<CString> *) m_IndexVector[Index];
    CString Result;
    CString Slash;
    while (Node) {
        if (Node != (CTreeElement<CString> *) m_IndexVector[Index]) {
            Slash = '/';
        } else {
            Slash.Empty();
        }
        Result = Node->GetElement() + Slash + Result;
        Node = Node->GetParent();
    }
    return Result;
}

bool CUrlTree::Load(const CString& FileName, bool Verbose) {
    RemoveAll();
  
    CMMapFile FastFile(FileName);

    if (! FastFile.MMap(MMAP_READOPENMODE))
      return false;

    long fSize = FastFile.GetSize();
    if (fSize && FastFile.GetMem()) {
        m_Progress.Init(10, Verbose);
        CString Line;
        while (FastFile.ReadLine(&Line) >= 0) {
            m_Progress.Show(FastFile.GetOffset(), fSize, Verbose);
            Add(Line);
        }
        m_Progress.Finish(Verbose);
    }
    if (Verbose) 
        cout << "[" << GetSize() << " lines]" << endl;
    return true;
}

bool CUrlTree::AddFast(const CString& Line, CVector<CObject *>& HorizontalVector) {
    // cout << "Line: " << Line << endl;
    CVector<CString> SpaceVector;
    CString::StrToVector(Line, ' ', &SpaceVector);
    if (SpaceVector.GetSize() != 3) return false;
    int AbstractPosition = CString::StrToInt(SpaceVector[0]);
    int HorizontalPosition = CString::StrToInt(SpaceVector[1]);
    SpaceVector[2] = CUrl::UnEscape(SpaceVector[2]);    
    // cout << "Abstract/Horizontal: " << AbstractPosition << "/" << HorizontalPosition << endl;
    CObject * NewNode = NULL;
    if (!HorizontalVector.GetSize() && !HorizontalPosition) {
        /* top node */
        NewNode = (CObject *) (m_Head = new CTreeElement<CString>(SpaceVector[2]));
        HorizontalVector += NewNode;
    } else if (HorizontalPosition == ((int)HorizontalVector.GetSize() - 1)) {
        /* same level, add after element */
        NewNode = (CObject *) AddAfter((CTreeElement<CString> *) HorizontalVector[HorizontalPosition], SpaceVector[2]);
        HorizontalVector[HorizontalPosition] = NewNode;
    } else if (HorizontalPosition > ((int)HorizontalVector.GetSize() - 1)) {
        /* sub level, add child */
        NewNode = (CObject *) AddChildLast((CTreeElement<CString> *) HorizontalVector[HorizontalPosition-1], SpaceVector[2]);
        HorizontalVector += NewNode;
    } else if (HorizontalPosition < ((int)HorizontalVector.GetSize() - 1)) {
        /* previous level, remove and add */
        do {
            HorizontalVector.RemoveAt((int)HorizontalVector.GetSize() - 1);
        } while (HorizontalPosition < ((int) HorizontalVector.GetSize() - 1));
        NewNode = (CObject *) AddAfter((CTreeElement<CString> *) HorizontalVector[HorizontalPosition], SpaceVector[2]);
        HorizontalVector[HorizontalPosition] = NewNode;
    } else return false;

    // cout << "New Tree:" << endl << (* this) << endl;
    
    /* set the absolute URL index */
    if (AbstractPosition != -1) {
        m_IndexVector.SetDim(AbstractPosition + 1);        
        while ((int) m_IndexVector.GetSize() <= AbstractPosition) 
            m_IndexVector.Add((CObject *) NULL);
        m_IndexVector[AbstractPosition] = NewNode;                
        m_UrlVector.SetSize(m_IndexVector.GetSize());
    	m_UrlVector[AbstractPosition] = ResolveAt(AbstractPosition);
    } 
    
    ((CTreeElement<CString> *) NewNode)->SetData((void *) AbstractPosition);
    
    return true;
}

bool CUrlTree::FastLoad(const CString& FileName, bool Verbose) {
    RemoveAll();
    
    CMMapFile FastFile(FileName);

    if (! FastFile.MMap(MMAP_READOPENMODE))
      return false;
    
    long fSize = FastFile.GetSize();
    if (fSize && FastFile.GetMem()) {
        m_Progress.Init(10, Verbose);
        CVector<CObject *> HorizontalVector;
        CString Line;        
        while (FastFile.ReadLine(&Line) >= 0) {
            m_Progress.Show(FastFile.GetOffset(), fSize, Verbose);
            if (!AddFast(Line, HorizontalVector)) {
                m_Progress.Error(Verbose);
                return false;
            }
        }
        m_Progress.Finish(Verbose);
    }
    
    if (Verbose) {
        cout << "[" << GetSize() << " lines]" << endl;
    }
    
    return true;
}

bool CUrlTree::SaveNode(FILE * Stream, CTreeElement<CString> * Element, int Level, bool Verbose, long& Total, int DataRows) const {
    int NodeIndex;
    while (Element) {
        NodeIndex = (int) Element->GetData();
        if (NodeIndex != -1)
            Total++;
        if (!DataRows || (DataRows > NodeIndex)) {	  
            m_Progress.Show(Total, GetSize(), Verbose);
            if (! fprintf(Stream, "%d %d %s%s", 
                NodeIndex, 
                Level, 
                CUrl::Escape(Element->GetElement(), true).GetBuffer(), 
                g_strCrLf)) {
                return false;
            }
        }
        if (Element->GetChild()) {
            if (! SaveNode(Stream, Element->GetChild(), Level+1, Verbose, Total, DataRows)) {
                return false;
            }
        }
        Element = Element->GetNext();
    }
    return true;
}

bool CUrlTree::Save(const CString& FileName, bool Verbose, int DataRows) const {
    FILE * OStream2 = fopen((const char *) FileName.GetBuffer(), "wb+");
    if (OStream2) {
        long Total = 0;
        m_Progress.Init(10, Verbose);
        if (! SaveNode(OStream2, m_Head, 0, Verbose, Total, DataRows)) {
            m_Progress.Error(Verbose);
        } else {
            m_Progress.Finish(Verbose);
        }
        fclose(OStream2);
        return true;
    } else {
        return false;  
    }
}

int CUrlTree::FindNodeIndexes(CTreeElement<CString> * Element, CIntVector& Result, bool SameLevel) const {
    if (!Element) 
        return -1;
    
    // _L_DEBUG(3, cout << "looking at node: [" << Element->GetElement() << "]" << endl);
    
    if (Element->GetChild()) {
        FindNodeIndexes(Element->GetChild(), Result, true); 
    }
    
    if (SameLevel && Element->GetNext()) {
        FindNodeIndexes(Element->GetNext(), Result, true);
    }

    int NodeIndex = -1;

    NodeIndex = (int) Element->GetData();

    if (NodeIndex != -1) {        
        Result += NodeIndex;
    }
     
    return NodeIndex;
}

int CUrlTree::FindNodeIndexes(const CVector<CString>& UrlVector, CIntVector& Result) const { 
    CTreeElement<CString> * Element = Find(UrlVector);
    if (!Element)
        return -1;
    // _L_DEBUG(3, cout << "found element:[" << Element->GetElement() << "]" << endl);
    if (!Element->GetElement().GetLength() && Element->GetParent())
        Element = Element->GetParent();
    return FindNodeIndexes(Element, Result); 
}

int CUrlTree::FindNodeIndex(CTreeElement<CString> * Element) const {
    // _L_DEBUG(4, cout << "CUrlTree::FindNodeIndex(CTreeElement)" << endl);
    for (register int i=((int)m_IndexVector.GetSize())-1;i>=0;i--) {
        if ((CTreeElement<CString> *) m_IndexVector[i] == Element) {
            // _L_DEBUG(4, cout << "CUrlTree::FindNodeIndex(CTreeElement) - " << i << endl);
            return i;
        }
    }
    // _L_DEBUG(4, cout << "CUrlTree::FindNodeIndex(CTreeElement) - not found." << endl);
    return -1;
}
