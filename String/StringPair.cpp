/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>

#include "StringPair.hpp"

CStringPair::CStringPair(void) : CObject() {
    // _L_DEBUG(1, cout << "CStringPair :: CStringPair" << endl);
}

CStringPair::CStringPair(const CString& Name, const CString& Value) : CObject() {
    // _L_DEBUG(1, cout << "CStringPair :: CStringPair [" << Name << "/" << Value << "]" << endl);
	m_Name = Name;
	m_Value = Value;
}

CStringPair::CStringPair(const CStringPair& Pair) {
    // _L_DEBUG(1, cout << "CStringPair :: CStringPair [" << Pair.m_Name << "/" << Pair.m_Value << "]" << endl);
	operator=(Pair);
}

CStringPair& CStringPair::operator=(const CStringPair& Pair) {
    // _L_DEBUG(1, cout << "CStringPair :: operator= [" << Pair.m_Name << "/" << Pair.m_Value << "]" << endl);
	m_Name = Pair.m_Name;
	m_Value = Pair.m_Value;
	return * this;
}

CStringPair::~CStringPair(void) {

}
