/*

    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#ifdef _WIN32
#include "NTService.hpp"

CNTService * CNTService::g_NTService = NULL;

CNTService::CNTService(void) {
    g_NTService = this;
    m_Debug = false;
}

CNTService::~CNTService(void) {

}

void CNTService::Dispatch(void) {

  SERVICE_TABLE_ENTRYA ServiceDispatchTable[2];
  memset(ServiceDispatchTable, 0, sizeof(SERVICE_TABLE_ENTRYA) * 2);

  ServiceDispatchTable[0].lpServiceName = (char *) m_ServiceName.GetBuffer();
  ServiceDispatchTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA) ServiceMain;

  if (!StartServiceCtrlDispatcherA(ServiceDispatchTable)) {
      // report some kind of error to the event log
      CHandler :: Terminate(GetLastError());
  }
  
}

//
//  FUNCTION: service_main
//
//  PURPOSE: To perform actual initialization of the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls
//    the user defined ServiceStart() routine to perform majority
//    of the work.
//
void CNTService::SServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
  // register our service control handler:
  m_ServiceStatusHandle = RegisterServiceCtrlHandlerA( m_ServiceName.GetBuffer(), ServiceCtrl);
  if (m_ServiceStatusHandle) {

      m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
      m_ServiceStatus.dwServiceSpecificExitCode = 0;
      // report the status to the service control manager.
      if (ReportStatusToSCMgr(
          SERVICE_START_PENDING, // service state
          NO_ERROR,              // exit code
          3000)) {// wait hint
        
          ServiceStart();
      }
  }

  // try to report the stopped status to the service control manager.
  if (m_ServiceStatusHandle)
    ReportStatusToSCMgr(SERVICE_STOPPED, 0, 0);
}

//
//  FUNCTION: service_ctrl
//
//  PURPOSE: This function is called by the SCM whenever
//           ControlService() is called on this service.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//

void CNTService::SServiceCtrl(DWORD dwCtrlCode) {
  // Handle the requested control code.
    switch(dwCtrlCode) {
        // Stop the service.
        //
    case SERVICE_CONTROL_STOP:
        // Update the service status.
        ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
        ServiceStop();
        return;        
    case SERVICE_CONTROL_INTERROGATE:
        // invalid control code        
        break;        
    default:
        break;        
    }
    ReportStatusToSCMgr(m_ServiceStatus.dwCurrentState, NO_ERROR, 0);
}

//
//  FUNCTION: ReportStatusToSCMgr()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Manager
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
//  COMMENTS:
//
bool CNTService::ReportStatusToSCMgr(DWORD dwCurrentState,
                                     DWORD dwWin32ExitCode,
                                     DWORD dwWaitHint) {  
  
    static DWORD dwCheckPoint = 1;
    bool fResult = true;

    if (m_Debug)
        return fResult;

    if (dwCurrentState == SERVICE_START_PENDING)
        m_ServiceStatus.dwControlsAccepted = 0;
    else
        m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    
    m_ServiceStatus.dwCurrentState = dwCurrentState;
    m_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    m_ServiceStatus.dwWaitHint = dwWaitHint;
    
    if ( ( dwCurrentState == SERVICE_RUNNING ) ||
        ( dwCurrentState == SERVICE_STOPPED ) )
        m_ServiceStatus.dwCheckPoint = 0;
    else
        m_ServiceStatus.dwCheckPoint = dwCheckPoint++;
    
    // Report the status of the service to the service control manager.
    if (!(fResult = SetServiceStatus( m_ServiceStatusHandle, &m_ServiceStatus))) {
        // report some kind of error to the event log
    }
    return fResult;
}

bool CNTService::Install(void) {

    SC_HANDLE   ServiceHandle;
    SC_HANDLE   ServiceManager;

    CString ModuleFilename;
    DWORD dwLen = MAX_PATH;
    ModuleFilename.SetLength(dwLen);    
    if (!(dwLen = GetModuleFileNameA(GetModuleHandle(NULL),
        (char *) ModuleFilename.GetBuffer(), 
        dwLen))) {
        cout << "Error getting module path." << endl;
        CObject::ShowLastError();
        return false;
    } else {
        ModuleFilename.SetLength(dwLen);
    }    

    cout << "Attempting to install " << m_ServiceName << " (" << ModuleFilename << ") ..." << endl;
    
    ServiceManager = OpenSCManagerA(
        NULL,                   // machine (NULL == local)
        NULL,                   // database (NULL == default)
        SC_MANAGER_ALL_ACCESS   // access required
        );

    if ( !ServiceManager ) {
        cout << "OpenSCManager failed." << endl;
        CObject::ShowLastError();
        return false;
    }


    CString ServiceDependencies;
    for (register int i=0; i < (int) m_Dependencies.GetSize(); i++) {
        ServiceDependencies += m_Dependencies[i];
        ServiceDependencies += (char) 0;
    }
    ServiceDependencies += (char) 0;
        
    ModuleFilename += " service dispatch";
	ModuleFilename += (" --servicename=" + m_ServiceName);
	ModuleFilename += (" --servicedisplayname=" + m_ServiceDisplayName);

    ServiceHandle = CreateServiceA(
        ServiceManager,                         // SCManager database
        m_ServiceName.GetBuffer(),              // name of service
        m_ServiceDisplayName.GetBuffer(),       // name to display
        SERVICE_ALL_ACCESS,                     // desired access
        SERVICE_WIN32_OWN_PROCESS,              // service type
        SERVICE_AUTO_START,                     // start type
        SERVICE_ERROR_NORMAL,                   // error control type
        ModuleFilename.GetBuffer(),             // service's binary
        NULL,                                   // no load ordering group
        NULL,                                   // no tag identifier
        ServiceDependencies.GetBuffer(),        // dependencies
        m_RunasUsername.GetLength() ? m_RunasUsername.GetBuffer() : NULL,  // account
        m_RunasPassword.GetLength() ? m_RunasPassword.GetBuffer() : NULL); // password

	/* not supported on NT4
	// set service description if any
	DWORD dwSystemVersion = GetVersion();

	if (ServiceHandle && 
		m_ServiceDescription.GetLength() && 
		(dwSystemVersion  < 0x80000000) && // Windows 2000, XP, NT
		(LOBYTE(LOWORD(dwSystemVersion)) >= 5) // Windows 2000, XP
		) {
		SERVICE_DESCRIPTIONA ServiceDescription;
		ServiceDescription.lpDescription = (char *) m_ServiceDescription.GetBuffer();

		if (! ChangeServiceConfig2(ServiceHandle, SERVICE_CONFIG_DESCRIPTION, & ServiceDescription)) {
			cout << "ChangeServiceConfig2 failed." << endl;
			CObject::ShowLastError();
			CloseServiceHandle( ServiceHandle );
			if (ServiceManager) CloseServiceHandle( ServiceManager );
			return false;
		}
	}
	*/

    if (! ServiceHandle) {        
		cout << "CreateService failed." << endl;
        CObject::ShowLastError();
		if (ServiceManager) CloseServiceHandle( ServiceManager );
        return false;
    } else {
		CloseServiceHandle( ServiceHandle );
	}

	if (ServiceManager) {
        CloseServiceHandle( ServiceManager );
	}

    cout << m_ServiceDisplayName << " successfuly installed." << endl;
    return true;
}

//
//  FUNCTION: CmdRemoveService()
//
//  PURPOSE: Stops and removes the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
bool CNTService::Remove(void) {
    SC_HANDLE   ServiceHandle;
    SC_HANDLE   ServiceManager;

    cout << "Attempting to remove " << m_ServiceName << " ..." << endl;

  
    ServiceManager = OpenSCManagerA(
        NULL,                   // machine (NULL == local)
        NULL,                   // database (NULL == default)
        SC_MANAGER_ALL_ACCESS   // access required
        );

    if ( !ServiceManager ) {
        cout << "OpenSCManager failed." << endl;
        CObject::ShowLastError();
        return false;
    }


    ServiceHandle = OpenServiceA(ServiceManager, m_ServiceName.GetBuffer(), SERVICE_ALL_ACCESS);

    if (!ServiceHandle) {
        cout << "OpenService failed." << endl;
        CObject::ShowLastError();
        CloseServiceHandle (ServiceManager);
        return false;
    }
    

    if ( ControlService( ServiceHandle, SERVICE_CONTROL_STOP, & m_ServiceStatus )) {
        cout << "Stopping " << m_ServiceDisplayName << endl;
        base_sleep(1);
        while( QueryServiceStatus( ServiceHandle, &m_ServiceStatus) ) {
            if ( m_ServiceStatus.dwCurrentState == SERVICE_STOP_PENDING ) {
                cout << '.';
                cout.flush();
                base_sleep(1);
            } else break;
        }

        if ( m_ServiceStatus.dwCurrentState == SERVICE_STOPPED )
            cout << m_ServiceDisplayName << " stopped." << endl;
        else
            cout << m_ServiceDisplayName << " could not be stopped." << endl;
    }

    // remove the service
    if( DeleteService( ServiceHandle ) ) {
        cout << m_ServiceDisplayName << " removed." << endl;            
    } else {
        cout << "DeleteService failed." << endl;
        CObject::ShowLastError();
        CloseServiceHandle (ServiceManager);
        CloseServiceHandle (ServiceHandle);
        return false;
    }
    
    CloseServiceHandle (ServiceManager);
    CloseServiceHandle (ServiceHandle);
    return true;
}

//
//  FUNCTION: CmdDebugService(int argc, char ** argv)
//
//  PURPOSE: Runs the service as a console application
//
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
bool CNTService::Debug(void) {
    cout << "Debuggin " << m_ServiceDisplayName << endl;
    SetConsoleCtrlHandler( ControlHandler, TRUE );
    ServiceStart();
    return true;
}

//
//  FUNCTION: CmdStartService()
//
//  PURPOSE: Starts the service
//
//  PARAMETERS:
//
//  lpszServerName - Target server name. NULL for local machine
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
bool CNTService::Start(void) {
    SC_HANDLE   ServiceHandle;
    SC_HANDLE   ServiceManager;

    cout << "Attempting to start " << m_ServiceName << " ..." << endl;

    ServiceManager = OpenSCManagerA(
        NULL,                   // machine (NULL == local)
        NULL,                   // database (NULL == default)
        SC_MANAGER_ALL_ACCESS   // access required
        );

    if ( !ServiceManager ) {
        cout << "OpenSCManager failed." << endl;
        CObject::ShowLastError();
        return false;
    }


    ServiceHandle = OpenServiceA(ServiceManager, m_ServiceName.GetBuffer(), SERVICE_ALL_ACCESS);

    if (!ServiceHandle) {
        cout << "OpenService failed." << endl;
        CObject::ShowLastError();
        CloseServiceHandle (ServiceManager);
        return false;
    }
    

    if ( StartService( ServiceHandle, 0, NULL )) {
        cout << "Starting " << m_ServiceDisplayName << endl;
        base_sleep(1);
        while( QueryServiceStatus( ServiceHandle, &m_ServiceStatus) ) {
            if ( m_ServiceStatus.dwCurrentState == SERVICE_START_PENDING ) {
                cout << '.';
                cout.flush();
                base_sleep(1);
            } else break;
        }

        if ( m_ServiceStatus.dwCurrentState == SERVICE_RUNNING )
            cout << m_ServiceDisplayName << " started." << endl;
        else
            cout << m_ServiceDisplayName << " could not be started." << endl;
    }
    
    CloseServiceHandle (ServiceManager);
    CloseServiceHandle (ServiceHandle);
    return true;
}

//
//  FUNCTION: CmdControlService()
//
//  PURPOSE: Controls the service
//
//  PARAMETERS:
//
//  lpszServerName - Target server name. NULL for local machine
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
bool CNTService::Control(DWORD dwControl) {
    SC_HANDLE   ServiceHandle;
    SC_HANDLE   ServiceManager;

    switch(dwControl) {
    case SERVICE_CONTROL_STOP: cout << "Attempting to stop " << m_ServiceDisplayName << " ..." << endl; break;
    case SERVICE_CONTROL_PAUSE: cout << "Attempting to suspend " << m_ServiceDisplayName << " ..." << endl; break;
    case SERVICE_CONTROL_CONTINUE: cout << "Attempting to resume " << m_ServiceDisplayName << " ..." << endl; break;
    }

    
    ServiceManager = OpenSCManagerA(
        NULL,                   // machine (NULL == local)
        NULL,                   // database (NULL == default)
        SC_MANAGER_ALL_ACCESS   // access required
        );

    if ( !ServiceManager ) {
        cout << "OpenSCManager failed." << endl;
        CObject::ShowLastError();
        return false;
    }

    ServiceHandle = OpenServiceA(ServiceManager, m_ServiceName.GetBuffer(), SERVICE_ALL_ACCESS);

    if (!ServiceHandle) {
        cout << "OpenService failed." << endl;
        CObject::ShowLastError();
        CloseServiceHandle (ServiceManager);
        return false;
    }
    

    if ( !ControlService( ServiceHandle, dwControl, &m_ServiceStatus)) {
        cout << "ControlService failed." << endl;
        CObject::ShowLastError();
        CloseServiceHandle (ServiceManager);
        CloseServiceHandle (ServiceHandle);
        return false;

    }
        
    switch(dwControl) {
    case SERVICE_CONTROL_STOP: cout << "Stopping " << m_ServiceDisplayName << " ..." << endl; break;
    case SERVICE_CONTROL_PAUSE: cout << "Suspending " << m_ServiceDisplayName << " ..." << endl; break;
    case SERVICE_CONTROL_CONTINUE: cout << "Resuming " << m_ServiceDisplayName << " ..." << endl; break;
    }
    
    base_sleep(1);
    while( QueryServiceStatus( ServiceHandle, &m_ServiceStatus) ) {
        if (((dwControl == SERVICE_CONTROL_STOP) && (m_ServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)) ||
            ((dwControl == SERVICE_CONTROL_PAUSE) && (m_ServiceStatus.dwCurrentState == SERVICE_PAUSE_PENDING)) ||
            ((dwControl == SERVICE_CONTROL_CONTINUE) && (m_ServiceStatus.dwCurrentState == SERVICE_CONTINUE_PENDING))
            ) {                
            cout << '.';
            cout.flush();
            base_sleep(1);
        } else break;
    }
    
    switch(dwControl)
    {
    case SERVICE_CONTROL_STOP : 
        if (m_ServiceStatus.dwCurrentState == SERVICE_STOPPED) 
            cout << m_ServiceDisplayName << " stopped." << endl;						
        else 
            cout << m_ServiceDisplayName << " failed to stop." << endl;
        break;
    case SERVICE_CONTROL_PAUSE:
        if (m_ServiceStatus.dwCurrentState == SERVICE_PAUSED) 
            cout << m_ServiceDisplayName << " suspended." << endl;						
        else 
            cout << m_ServiceDisplayName << " failed to suspend." << endl;
        
        break;
    case SERVICE_CONTROL_CONTINUE: 
        if (m_ServiceStatus.dwCurrentState == SERVICE_RUNNING) 
            cout << m_ServiceDisplayName << " resumed." << endl;						
        else 
            cout << m_ServiceDisplayName << " failed to resume." << endl;                    
        break;
    }

    CloseServiceHandle (ServiceManager);
    CloseServiceHandle (ServiceHandle);
    return true;
}

//
//  FUNCTION: ControlHandler ( DWORD dwCtrlType )
//
//  PURPOSE: Handled console control events
//
//  PARAMETERS:
//    dwCtrlType - type of control event
//
//  RETURN VALUE:
//    True - handled
//    False - unhandled
//
//  COMMENTS:
//
bool CNTService::SControlHandler ( DWORD dwCtrlType )
{
    switch( dwCtrlType )
    {
    case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
    case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
        cout << "Stopping " << m_ServiceDisplayName << endl;      
        ServiceStop();
        return true;
        break;
    }
    return false;
}

#endif
