/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-2001 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#ifndef EMAIL_MIMEBODY_HPP
#define EMAIL_MIMEBODY_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
    
class CMimeBody
{
    property(CString, Body);
    
public:
    
    bool Load(const CString& Filename);
    
    CMimeBody(void);
    ~CMimeBody(void);
};

    
#endif
