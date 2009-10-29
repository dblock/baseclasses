/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "XmlTree.hpp"
#include <Internet/HtmlParser.hpp>

CXmlNode::CXmlNode(void) {      
  m_Type = xmlnUnknown;
}

CXmlNode::CXmlNode(const CXmlNode& XmlNode) {
  operator=(XmlNode);
}

CXmlNode::~CXmlNode(void) {
  
}

CString CXmlNode::GetQuotedData(void) const {
  CString QuotedData;
  CHtmlParser::Quote(m_Data, & QuotedData);
  QuotedData.Replace(" ", "&nbsp;");
  return QuotedData;
}

void CXmlNode::GetXml(CString& ssResult) const {
  switch (m_Type) {
  case xmlnUnknown:
    ssResult += GetQuotedData();
    break;
  case xmlnOpen:
    ssResult += '<';
    ssResult += GetQuotedData();
    ssResult += '>';
    break;
  case xmlnClose:
    ssResult += "</";
    ssResult += GetQuotedData();
    ssResult += '>';
    break;
  case xmlnData:
    ssResult += GetQuotedData();
    break;
  }
}

bool CXmlTree::ReadString(const CString& String, long& CurPos, CString& ssResult) {
  long PrevPos = CurPos;
  while ((CurPos < (long) String.GetLength())&&(String[CurPos] != '>') && (String[CurPos] != '<'))
    CurPos++;
  String.Mid(PrevPos, CurPos - PrevPos, &ssResult); 
  return true;
}

bool CXmlTree::ReadTag(const CString& String, long& CurPos, CXmlNode& Tag) {
  Tag.m_Type = xmlnUnknown;
  // end of string?
  if (CurPos >= (long) String.GetLength())
    return false;
  // skip spaces
  while ((CurPos < (int) String.GetLength()) && (String[CurPos] == ' '))
    CurPos++;
  // read first char
  switch(String[CurPos]) {
  case '<':
    CurPos++;
    Tag.m_Type = xmlnOpen;
    // end of string?
    if (CurPos >= (long) String.GetLength())
      return false;
    // ending?
    switch(String[CurPos]) {
    case '/':
      CurPos++;
      Tag.m_Type = xmlnClose;
      break;
    }
    // read tag name
    ReadString(String, CurPos, Tag.m_Data);
    // end of string
    if (CurPos >= (long) String.GetLength())
      return false;
    // close tag?
    switch(String[CurPos]) {
    case '>':
      CurPos++;
      break;
    default:
      return false;
    }
    break;
  default:
    Tag.m_Type = xmlnData;
    ReadString(String, CurPos, Tag.m_Data);
    break;
  }
  return true;
}

bool CXmlTree::Parse(const CString& String, CTreeElement< CXmlNode > * Node) {
  long CurPos = 0;
  CXmlNode Tag;
  
  while (CurPos < (long) String.GetLength()) {
    // read a tag
    if (!ReadTag(String, CurPos, Tag))
      break;
    
    switch (Tag.m_Type) {
    case xmlnClose:
      Node = AddAfter(Node, Tag);
      if (Node->GetParent())
        Node = Node->GetParent();
      break;
    case xmlnOpen:
      if (Tag.GetSelfTerm())
        AddChildLast(Node, Tag);
      else Node = AddChildLast(Node, Tag);
      break;
    case xmlnData:    
      AddChildLast(Node, Tag);
      break;
    default:
      Node = AddAfter(Node, Tag);
      break;
    }
  }
  return true;
}

bool CXmlTree::GetXml(CString& ssResult, CTreeElement< CXmlNode > * Node) const {
  
  if (!Node)
    return false;
  
  bool bResult = true;
  Node->GetElement().GetXml(ssResult);
  if (Node->GetChild()) 
    bResult &= GetXml(ssResult, Node->GetChild());
  if (Node->GetNext()) 
    bResult &= GetXml(ssResult, Node->GetNext());
  return bResult;
}

bool CXmlTree::GetWml(CString& ssResult, CTreeElement< CXmlNode > * Node) const {
  
  if (!Node)
    return false;
  
  bool bResult = true;

  bool bRow = false;

  while (Node &&
		 Node->GetElement().GetType() == xmlnOpen &&
	     Node->GetChild() && 
		 Node->GetChild()->GetElement().GetType() == xmlnData) 
  {
	  if (! bRow) {
		  ssResult += "<p>\n<big>";
		  ssResult += Node->GetParent() ? Node->GetParent()->GetElement().GetQuotedData() : Node->GetElement().GetQuotedData();
		  ssResult += "</big>\n<table columns=\"2\">";
	  }

	  // one child, data
	  ssResult += "<tr><td><u>";
	  ssResult += Node->GetElement().GetQuotedData();
	  ssResult += "</u></td><td><b>";
	  ssResult += Node->GetChild()->GetElement().GetQuotedData();
	  ssResult += "</b></td></tr>\n";

	  Node = Node->GetNext();
	  // this was a closing node
	  if (Node) Node = Node->GetNext();

	  bRow = true;
  }

  if (bRow) ssResult += "</table>\n</p>";

  if (Node && Node->GetChild()) 
    bResult &= GetWml(ssResult, Node->GetChild());
  if (Node && Node->GetNext()) 
    bResult &= GetWml(ssResult, Node->GetNext());
  return bResult;
}


CXmlTree::CXmlTree(void) {
  SetSorted(true);
  SetUnique(true);
}

CXmlTree::~CXmlTree(void) {
  
}  

bool CXmlTree::SetXml(const CString& String) {
  bool bResult = true;
  
  if (!String.GetLength())
    return false;
  
  CXmlNode NewElement;
  if (m_Head)
    RemoveAll(m_Head);
  m_Head = ::new CTreeElement< CXmlNode >(NewElement);
  if (!Parse(String, m_Head))
    bResult = false;
  return bResult;
}

CTreeElement< CXmlNode > * CXmlTree::XmlFind(const CString& String) const {
  CVector<CString> Vector;
  CString::StrToVector(String, '/', &Vector);
  return XmlFind(Vector);
}

CTreeElement< CXmlNode > * CXmlTree::XmlFind(const CVector< CString >& EltArray, bool bCreate) const {
  unsigned int CurrentIndex = 0;
  CTreeElement< CXmlNode > * Current = m_Head;
  CTreeElement< CXmlNode > * Saved = Current;
  while (Current) {
    if (CurrentIndex >= EltArray.GetSize()) 
      return Current;
    // cout << "Comparing " << Current->GetElement().m_Data << " with " << EltArray[CurrentIndex] << endl;
    if (Current->GetElement().m_Data.StartsWith('?')) {
      Saved = Current;
      Current = Current->GetChild();
      if (!EltArray[CurrentIndex].GetLength())
        CurrentIndex++;
      continue;
    }
    if ((Current->GetElement().m_Data == EltArray[CurrentIndex]) ||
        ((Current->GetElement().m_Data.GetLength() > EltArray[CurrentIndex].GetLength()) && 
         (Current->GetElement().m_Data[EltArray[CurrentIndex].GetLength()] == ' ')))        
      {
        // cout << "ok" << endl;
        CurrentIndex++;
        if (CurrentIndex == EltArray.GetSize()) {
          return Current;
        } else {
          Saved = Current;
          Current = Current->GetChild();
        }
      } else Current = Current->GetNext();
  }
  
  if (!bCreate)
    return NULL;
  
  for (register int i=CurrentIndex;i<(int)EltArray.GetSize();i++) {
    CXmlNode XmlNode;
    XmlNode.m_Type = xmlnOpen;
    XmlNode.m_Data = EltArray[CurrentIndex];
    Saved = ((CXmlTree *) this)->AddChildLast(Saved, XmlNode);
    XmlNode.m_Type = xmlnClose;
    ((CXmlTree *) this)->AddAfter(Saved, XmlNode);
  }
  return Saved;
} 

bool CXmlTree::SetValue(const CString& Path, const CString& Value) {
  CVector<CString> ssStrArray;
  CString::StrToVector(Path, '/', &ssStrArray);
  
  CTreeElement< CXmlNode > * Node = XmlFind(ssStrArray, true);
  if (!Node) {    
    return false;
  }
  
  CXmlNode NewElement;
  NewElement.m_Data = Value;
  NewElement.m_Type = xmlnData;                
  
  if (Node->GetChild())
    * Node->GetChild() = NewElement;
  else AddChildLast(Node, NewElement);
  
  return true;
}

CString CXmlTree::GetValueSS(const CString& Path) const {
  CString ssResult;
  GetValue(Path, ssResult);
  return ssResult;  
}
  
bool CXmlTree::GetValue(const CString& Path,  CString& Value) const {
  CVector<CString> ssStrArray;
  CString::StrToVector(Path, '/', &ssStrArray);
  CTreeElement< CXmlNode > * Node = XmlFind(ssStrArray);
  if (!Node)
    return false;
  if (Node->GetChild())
    Value = Node->GetChild()->GetElement().m_Data;
  return true;
}

CString CXmlTree::GetXmlSS(void) const {
  CString ssResult;
  if (m_Head && m_Head->GetChild())
    GetXml(ssResult, m_Head->GetChild());
  return ssResult;
}

bool CXmlTree::GetXml(CString& ssResult) const {
  if (!m_Head || !m_Head->GetChild())
	  return false;

  ssResult.Empty();

  return GetXml(ssResult, m_Head->GetChild());
}

bool CXmlTree::GetWml(CString& ssResult) const {
  if (!m_Head || !m_Head->GetChild())
	  return false;

  ssResult.Empty();

  return GetWml(ssResult, m_Head->GetChild());  
}

