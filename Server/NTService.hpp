/*

    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <platform/include.hpp>

#ifdef _WIN32
#ifndef BASE_NT_SERVICE_HPP
#define BASE_NT_SERVICE_HPP

class CNTService : public CObject {
private:
    property(bool, Debug);
    readonly_property(SERVICE_STATUS, ServiceStatus);
    readonly_property(SERVICE_STATUS_HANDLE, ServiceStatusHandle);    
    property(CString, ServiceName);
    property(CString, ServiceDisplayName);    
	property(CString, ServiceDescription);
    property(CVector<CString>, Dependencies);
	property(CString, RunasUsername);
	property(CString, RunasPassword);
public:
    
    static CNTService * g_NTService;

    CNTService(void);
    virtual ~CNTService(void);

    void Dispatch(void);

    bool Install(void);    
    bool Remove(void);
    bool Start(void);
    inline bool Stop(void) { return Control(SERVICE_CONTROL_STOP); }
    inline bool Pause(void) { return Control(SERVICE_CONTROL_PAUSE); }
    inline bool Resume(void) { return Control(SERVICE_CONTROL_CONTINUE); }
    bool Control(DWORD dwControl);    
    bool Debug(void);

    // set the current status of the service and report it to the Service Control Manager
    bool ReportStatusToSCMgr(
        DWORD dwCurrentState,   // the current state of service
        DWORD dwWin32ExitCode,  // error code to report
        DWORD dwWaitHint        // worst case estimate to next checkpoint
    );   
private:
    void SServiceMain( DWORD dwArgc, LPTSTR * lpszArgv );
    void SServiceCtrl( DWORD dwCtrlCode );
    bool SControlHandler ( DWORD dwCtrlType );
public:
    inline static void WINAPI ServiceMain( DWORD dwArgc, LPTSTR * lpszArgv ) {
        g_NTService->SServiceMain( dwArgc, lpszArgv);
    }
    inline static void WINAPI ServiceCtrl( DWORD dwCtrlCode ) {
        g_NTService->SServiceCtrl( dwCtrlCode );
    }
    inline static BOOL WINAPI ControlHandler ( DWORD dwCtrlType ) {
        return g_NTService->SControlHandler( dwCtrlType );
    }
    inline CString GetRegParametersPath(void) const {
        return "SYSTEM\\CurrentControlSet\\Services\\" + m_ServiceName + "\\Parameters";
    }
};


/*

    The service should use ReportStatusToSCMgr to indicate
    progress.  This routine must also be used by StartService()
    to report to the SCM when the service is running.
    
    If a ServiceStop procedure is going to take longer than
    3 seconds to execute, it should spawn a thread to
    execute the stop code, and return.  Otherwise, the
    ServiceControlManager will believe that the service has
    stopped responding.

*/

void ServiceStart(void);
void ServiceStop(void);


#endif
#endif