/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "Authentication.hpp"

CAuthenticator :: CAuthenticator(CAuthenticationType AuthType) :     
    m_AuthType(AuthType) {

}

CAuthenticator :: ~CAuthenticator(void) {
    
}

