/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-2001 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#ifndef EMAIL_MIMEHEADER_HPP
#define EMAIL_MIMEHEADER_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <String/StringTable.hpp>

typedef enum {
    btBoundaryDeclare,
    btBoundarySeparate,
    btBoundaryTerminate    
} CMimeBoundaryType;

class CMimeHeader
{
    property(CStringTable, MimeHeaders);
public:        
    bool Add(const CString& ssName, const CString& ssValue);
    bool Set(const CString& ssName, const CString& ssValue);
    bool Delete(const CString& ssName);
    bool Get(const CString& ssName, CString * pssValue, bool * pbFound = NULL) const;
    bool Dump(CString * pssValue, bool bOverwrite = true) const;
    static bool GetBoundary(CString * pssBoundaryValue, CMimeBoundaryType BoundaryType, CString * pssBoundaryString = NULL);
    static bool GetBoundary(CString * pssBoundaryValue);
    CMimeHeader(void);
    ~CMimeHeader(void);   
};

#endif
