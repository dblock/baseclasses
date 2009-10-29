/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
    written by Daniel Doubrovkine - dblock@vestris.com
    
*/

#include <baseclasses.hpp>

#include "StringTable.hpp"


CStringTable::CStringTable(void) {
    
}

CStringTable::~CStringTable(void) {
    
}

CStringTable::CStringTable(const CStringTable& Table) {
    operator=(Table);
}

CStringTable& CStringTable::operator=(const CStringTable& Table) {
    m_Table = Table.m_Table;
    return * this;
}

void CStringTable::Add(const CStringTable& StringTable) {
    m_Table.Add(StringTable.m_Table);	
}

void CStringTable::Add(const CString& Name, const CString& Value) {
    CStringPair Pair(Name, Value);
    m_Table.Add(Pair);
}

void CStringTable::InsertAt(const unsigned int Index, const CString& Name, const CString& Value) {
    CStringPair Pair(Name, Value);
    m_Table.InsertAt(Index, Pair);
}

void CStringTable::Set(const CString& Name, const CString& Value) {
    Remove(Name);
    Add(Name, Value);
}

void CStringTable::SetAt(const unsigned int Index, const CString& Name, const CString& Value) {
    assert(m_Table.GetSize() > Index);
    
    m_Table[Index].SetName(Name);
    m_Table[Index].SetValue(Value);
    
}

CStringPair& CStringTable::operator[](const unsigned int Index) {
    assert(m_Table.GetSize() > Index);
    return m_Table[Index];
}

const CStringPair& CStringTable::operator[](const unsigned int Index) const {
    assert(m_Table.GetSize() > Index);
    return m_Table[Index];
}

CString CStringTable::GetValue(const CString& Name) const {
    int Pos = -1;
    
    for (register int i=(int) m_Table.GetSize() - 1; i>=0;i--) {
        if (m_Table[i].GetName().Same(Name)) {
            Pos = i;
            break;
        }
    }
    
    
    if (Pos != -1)
        return m_Table[Pos].GetValue();
    
    return CString::EmptyCString;
}

bool CStringTable::FindAndCopy(const CString& Name, CString& Target) const {
    bool bResult = false;
    
    
    for (register int i=(int) m_Table.GetSize() - 1; i>=0;i--) {
        if (m_Table[i].GetName().Same(Name)) {
            Target = m_Table[i].GetValue();
            bResult = true;
            break;
        }
    }
    
    
    return bResult;
}

int CStringTable::FindName(const CString& Name) const {
    int Result = -1;
    
    for (register int i=(int) m_Table.GetSize() - 1; i>=0;i--) {
        if (m_Table[i].GetName().Same(Name)) {
            Result = i;
            break;
        }
    }
    
    return Result;
}

CString CStringTable::GetValueAt(const unsigned int Index) const {
    assert(m_Table.GetSize() > Index);
    return m_Table[Index].GetValue();
}

CString CStringTable::GetNameAt(const unsigned int Index) const {
    assert(m_Table.GetSize() > Index);
    return m_Table[Index].GetName();
}

void CStringTable::RemoveAll(void) {
    m_Table.RemoveAll();
}

void CStringTable::RemoveAt(const unsigned int Index) {
    assert(m_Table.GetSize() > Index);
    m_Table.RemoveAt(Index);
}

void CStringTable::Remove(const CString& Name) {
    
    for (register int i=(int) m_Table.GetSize() - 1; i>=0;i--) {
        if (m_Table[i].GetName().Same(Name)) 
            m_Table.RemoveAt(i);
    }
    
}

ostream& CStringTable::operator<<(ostream& Stream) const {
    
    for (register int i=0;i<(int)m_Table.GetSize();i++)
        Stream << m_Table[i];
    
    return Stream;
}

CString CStringTable::Get(void) const {
    CString Result;
    
    for (register int i=0;i<(int)m_Table.GetSize();i++) {
        if (i) {
            Result += ' ';
        }
        Result += m_Table[i].GetName();
        Result += '=';
        Result += m_Table[i].GetValue();
    }
    
    return Result;
}

CString& CStringTable::MapTerm(const CString& Format, CString& Target, int Dummy) const {
    MAP_TERM_MACRO(Format, Target, MapTerm, MapTermEach, false, Dummy);
}

CString CStringTable::Map(const CString& Format) const {
    CString Result;
    MapTerm(Format, Result);
    return Result;
}

void CStringTable::PopulateXmlNode(CXmlTree& Tree, CTreeElement< CXmlNode > * pXmlNode) const {  
    CXmlNode ConfigNode, DataNode;
    CString ValueData;
    
    for (int i=0;i<(int) GetSize();i++) {      
        ConfigNode.SetType(xmlnOpen);
        ConfigNode.SetData(GetNameAt(i));
        CTreeElement< CXmlNode > * pXmlConfigNode = Tree.AddChildLast(pXmlNode, ConfigNode);
        
        DataNode.SetType(xmlnData);
        ValueData = GetValueAt(i);
        ValueData.Replace("\\","\\\\");
        DataNode.SetData(ValueData);
        Tree.AddChildLast(pXmlConfigNode, DataNode);
        
        ConfigNode.SetType(xmlnClose);
        Tree.AddChildLast(pXmlNode, ConfigNode);
    }
    
}
