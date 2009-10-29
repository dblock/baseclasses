/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include <Server/PingClient.hpp>
#include <Object/Tracer.hpp>

CPingClient::CPingClient(void) {
    m_PingInterval = 30;
    m_Terminate = false;
}

CPingClient::CPingClient(const CString& PingUrl) {
    m_PingUrl = PingUrl;
    m_PingInterval = 30;
    m_bPing = true;
    m_Terminate = false;
}

void CPingClient::Execute(void * Arguments) {
    
    CHttpRequest HttpPingRequest;
    
    HttpPingRequest.SetFollowRedirections(false);
    HttpPingRequest.SetRequestSizeLimit(0);
    HttpPingRequest.SetRequestMethod(htHead);
    HttpPingRequest.SetTimeout(10);
    
    bool bResult = false;
    
    while(!m_Terminate) {
        
        if (m_bPing) {

            bResult = HttpPingRequest.Execute(m_PingUrl);
            
            Trace(tagServer, levInfo, ("CPingClient::Execute - status value: %d.", HttpPingRequest.GetRStatusValue()));
    
            if ((! bResult) || (HttpPingRequest.GetRStatusValue() == 0) || (HttpPingRequest.GetRStatusValue() >= 400)) {
                PingFailed(m_PingFailed.Inc());
            } else {
                PingSucceeded();          
                m_PingFailed.Set(0);
            }
        }
        
        base_sleep(m_PingInterval);
    }
    
    CThread::Execute(Arguments);
}

CPingClient::~CPingClient(void) {

}

void CPingClient::PingFailed(int /* Count */) const {
    
}

void CPingClient::PingSucceeded(void) const {
    
}
