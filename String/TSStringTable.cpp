/*
  
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>

#include "TSStringTable.hpp"

 
CTSStringTable::CTSStringTable(void) {
  
}

CTSStringTable::~CTSStringTable(void) {

}

CTSStringTable::CTSStringTable(const CTSStringTable& Table) {
	operator=(Table);
}

CStringTable& CTSStringTable::operator=(const CStringTable& Table) {
    m_RWMutex.StartWriting();
    CStringTable::operator=(Table);
    m_RWMutex.StopWriting();
	return * this;
}

CTSStringTable& CTSStringTable::operator=(const CTSStringTable& Table) {
    m_RWMutex.StartWriting();
    CStringTable::operator=(Table);
    m_RWMutex.StopWriting();
	return * this;
}

void CTSStringTable::Add(const CTSStringTable& StringTable) {
    m_RWMutex.StartWriting();
    CStringTable::Add(StringTable);
    m_RWMutex.StopWriting();
}

void CTSStringTable::Add(const CString& Name, const CString& Value) {
    m_RWMutex.StartWriting();
    CStringTable::Add(Name, Value);
    m_RWMutex.StopWriting();
}

void CTSStringTable::InsertAt(const unsigned int Index, const CString& Name, const CString& Value) {
    m_RWMutex.StartWriting();
    CStringTable::InsertAt(Index, Name, Value);
    m_RWMutex.StopWriting();
}
 
void CTSStringTable::Set(const CString& Name, const CString& Value) {    
    // this is remove and add
    CStringTable::Set(Name, Value);    
}

void CTSStringTable::SetAt(const unsigned int Index, const CString& Name, const CString& Value) {
    m_RWMutex.StartWriting();
    CStringTable::SetAt(Index, Name, Value);
    m_RWMutex.StopWriting();
}

CString CTSStringTable::GetValue(const CString& Name) const {
    m_RWMutex.StartReading();
    CString Result = CStringTable::GetValue(Name);
    m_RWMutex.StopReading();
    return Result;
}

bool CTSStringTable::FindAndCopy(const CString& Name, CString& Target) const {
    m_RWMutex.StartReading();
    bool bResult = CStringTable::FindAndCopy(Name, Target);
    m_RWMutex.StopReading();
    return bResult;
}

int CTSStringTable::FindName(const CString& Name) const {
    m_RWMutex.StartReading();
    int Result = CStringTable::FindName(Name);
    m_RWMutex.StopReading();
    return Result;
}

CString CTSStringTable::GetValueAt(const unsigned int Index) const {
	m_RWMutex.StartReading();
    CString Result = CStringTable::GetValueAt(Index);
    m_RWMutex.StopReading();
    return Result;
}

CString CTSStringTable::GetNameAt(const unsigned int Index) const {
    m_RWMutex.StartReading();
    CString Result = CStringTable::GetNameAt(Index);
    m_RWMutex.StopReading();
    return Result;
}

void CTSStringTable::RemoveAll(void) {
    m_RWMutex.StartWriting();
    CStringTable::RemoveAll();
    m_RWMutex.StopWriting();    
}

void CTSStringTable::RemoveAt(const unsigned int Index) {
    m_RWMutex.StartWriting();
    CStringTable::RemoveAt(Index);
    m_RWMutex.StopWriting();}

void CTSStringTable::Remove(const CString& Name) {
    m_RWMutex.StartWriting();
    CStringTable::Remove(Name);
    m_RWMutex.StopWriting();
}

ostream& CTSStringTable::operator<<(ostream& Stream) const {
    m_RWMutex.StartReading();
    ostream& Result = CStringTable::operator<<(Stream);
    m_RWMutex.StopWriting();
    return Result;    
}

CString CTSStringTable::Get(void) const {
    m_RWMutex.StartWriting();
    CString Result = CStringTable::Get();
    m_RWMutex.StopWriting();
    return Result;
}
