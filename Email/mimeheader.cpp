/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-2001 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>
#include "mimeheader.hpp"

CMimeHeader :: CMimeHeader(void) {
    
}

CMimeHeader :: ~CMimeHeader(void) {
    
}

bool CMimeHeader :: Add(const CString& ssName, const CString& ssValue) {
    CString ssLowerCaseName(ssName);
    ssLowerCaseName.LowerCase();   
    m_MimeHeaders.Add(ssLowerCaseName, ssValue);
    return true;
}

bool CMimeHeader :: Set(const CString& ssName, const CString& ssValue) {
    CString ssLowerCaseName(ssName);
    ssLowerCaseName.LowerCase();   
    m_MimeHeaders.Set(ssLowerCaseName, ssValue);
    return true;
}

bool CMimeHeader :: Delete(const CString& ssName) {
    m_MimeHeaders.Remove(ssName);
    return true;
}

bool CMimeHeader :: Get(const CString& ssName, CString * pssValue, bool * pbFound) const {

    if (! pssValue)
        return false;
    
    bool bResult = m_MimeHeaders.FindAndCopy(ssName, * pssValue);
    
    if (pbFound)
        * pbFound = bResult;
    
    return true;
}

bool CMimeHeader :: Dump(CString * pssValue, bool bOverwrite) const {
    
    if (! pssValue)
        return false;
    
    if (bOverwrite)
        pssValue->Empty();

    for (int i = 0; i < (int) m_MimeHeaders.GetSize(); i++) {
        (* pssValue) += m_MimeHeaders.GetNameAt(i);
        (* pssValue) += ": ";
        (* pssValue) += m_MimeHeaders.GetValueAt(i);
        (* pssValue) += "\r\n";
    }

    return true;
}

bool CMimeHeader :: GetBoundary(CString * pssValue) {

    if (! pssValue)
        return false;
    
    time_t CurrentTime;
    time(& CurrentTime);
    srand(CurrentTime);
    
    unsigned int nHighPart = rand();
    unsigned int nLowPart = rand();
    
    pssValue->SetLength(sizeof("000_003A_01C09733.24DFD8F0") - 1); // sample boundary value
    
    base_snprintf(pssValue->GetData(), sizeof("000_003A_01C09733.24DFD8F0"), "000_%04x_%08x.%08x", rand() % 1024, nHighPart, nLowPart);
    
    return true;
}

bool CMimeHeader :: GetBoundary(CString * pssBoundaryValue, CMimeBoundaryType BoundaryType, CString * pssBoundaryString) {
    
    CString ssBoundaryString;
    
    if (! pssBoundaryValue)
        return false;
    
    if (! pssBoundaryString)
        return false;
    
    switch (BoundaryType)
    {
        
    case btBoundaryDeclare:
        
        (* pssBoundaryValue) = "boundary=\"----=_NextPart_";
        (* pssBoundaryValue) += (* pssBoundaryString);
        (* pssBoundaryValue) += "\"";
        break;
        
    case btBoundaryTerminate:

        (* pssBoundaryValue) = "------=_NextPart_";
        (* pssBoundaryValue) += (* pssBoundaryString);
        (* pssBoundaryValue) += "--\r\n";
        break;
        
    case btBoundarySeparate:
        
    default:
        
        (* pssBoundaryValue) = "------=_NextPart_";
        (* pssBoundaryValue) += (* pssBoundaryString);
        (* pssBoundaryValue) += "\r\n";
    }

    return true;
}

