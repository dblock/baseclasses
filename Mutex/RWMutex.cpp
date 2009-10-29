/*
    
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  original implementation Fulco Houkes - houkes@xo3.com
     © Xo3 S.A., Geneva, Switzerland    
     http://www.xo3.com, All Rights Reserved

  rewritten by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>    
#include <Mutex/RWMutex.hpp>

CRWMutex::CRWMutex (void) : m_Readers(0) {

}

CRWMutex::~CRWMutex () {

}
