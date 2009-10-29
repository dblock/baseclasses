/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-2001 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp> 
#include "mimebody.hpp"
#include <File/LocalFile.hpp>

CMimeBody :: CMimeBody(void) {
    
}

CMimeBody :: ~CMimeBody(void) {
    
}

bool CMimeBody :: Load(const CString& Filename) {
    CLocalFile BodyFile(Filename);
    
    if (! BodyFile.OpenReadBinary())
        return false;

    if (! BodyFile.Read(& m_Body))
        return false;
    
    return true;
}

