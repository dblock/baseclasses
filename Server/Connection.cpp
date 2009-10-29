/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "Connection.hpp"

CConnection::CConnection(void) : m_Server(NULL) {
  
}

#include <Server/Server.hpp>
 
void CConnection::Execute(void * /* Arguments */) {
    m_Socket.LingerSocket();
    if (!((CServer *) m_Server)->GetNagle())
        m_Socket.DisableNagle();        
    m_Socket.MakePeerInformation(false);    
    ((CServer *) m_Server)->ExecuteClient(* this);    
    Close();
}

CConnection::~CConnection(void) {
	
}
