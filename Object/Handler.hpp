/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_HANDLER_HPP
#define BASE_HANDLER_HPP

#include <platform/include.hpp>
#include <Object/Tracer.hpp>

class CHandler {
    bool m_Initialized;
    property(bool, SignalSigterm);
public:
private:
#ifdef _WIN32
	static int OutOfMemory(size_t);
#endif
#ifdef _UNIX	
	static void OutOfMemory(void);
#endif
	static void SignalSigterm(int);
    static void RemoveTmpFiles(void);
public:
	CHandler(void);
	virtual ~CHandler(void);	
#ifdef _WIN32
	static void ShowLastError(void);
#endif
#ifdef _UNIX
    static inline void ShowLastError(void) { }
#endif
    void Initialize();
    static void Terminate(int nResult = 0);
};

extern CHandler * g_pHandler;
    
void GlobalInitialize(bool bEnableSwap, char * pFilename);
void GlobalTerminate(void);  

#endif

