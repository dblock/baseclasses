/*
	© Vestris Inc., Geneva, Switzerland
	http://www.vestris.com, 1994-1999 All Rights Reserved
	_____________________________________________________

	written by Daniel Doubrovkine - dblock@vestris.com
	and Serge Huber - huber@xo3.com

	*/

#include "Orb.hpp"

bool COrb::AddObject(const CString& ObjectName, CObject* ObjectPtr) {
	bool Result  = m_ObjectTable.Set(ObjectName, ObjectPtr);
	return Result;
}

bool COrb::RemoveObject(const CString& ObjectName) {
	bool Result = m_ObjectTable.Remove(ObjectName);
	return Result;
}

CObject * COrb::FindObject(const CString& ObjectName) const {
	CObject ** Result = (CObject **) m_ObjectTable.FindElement(ObjectName);
	if (Result) return (* Result);
	else return NULL;
}

// Object repository procedures
COrb::COrb(void) {

}

COrb::~COrb(void) {
	m_ObjectTable.RemoveAll();
}
