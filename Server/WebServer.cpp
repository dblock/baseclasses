/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "WebServer.hpp"

void CWebServer::ExecuteClient(const CConnection& Connection) {
    bool bKeepAliveSession = true;
    do {
        
        CHttpIo HttpIo(Connection);
    
        Trace(tagServer, levInfo, 
              ("CWebServer::ExecuteClient - [%s] %s (%s).", 
               HttpIo.GetRequestMethodString().GetBuffer(),
               HttpIo.GetRequestUrl().GetHttpAll().GetBuffer(), 
               HttpIo.GetMalformed() ? "bad" : "ok"));
        
        if (! HttpIo.GetMalformed()) {
            ExecuteWebClient(HttpIo);
        }
        
        bKeepAliveSession = HttpIo.GetKeepAlive();
        
        Trace(tagServer, levInfo, ("CWebServer::ExecuteClient - Keep-Alive: %s.", bKeepAliveSession ? "yes" : "no"));
        
    } while (bKeepAliveSession);
    
}

CWebServer::CWebServer(const int Port, const CString& Address) : CServer(Port, Address) {
    
}

void CWebServer::ExecuteWebClient(CHttpIo&) {
    
}
