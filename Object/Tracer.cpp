/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com        
       
*/

#include <baseclasses.hpp>

#include "Tracer.hpp"
#include <File/LocalPath.hpp>

#ifdef BASE_TRACE_ENABLED

bool s_TraceLevels[MAX_TRACE];
bool s_TraceTags[MAX_TRACE];

CInternalTrace :: CInternalTrace(long Tag, CTraceLevel Level, char * File, int Line) {
    m_Tag = Tag;
    m_Level = Level;
    m_File = strrchr(File, PATH_SEPARATOR);
    if (! m_File)
        m_File = File;
    else m_File++;
    m_Line = Line;
}

void CInternalTrace :: InternalTrace(char * Format, ...) {    
    va_list Args;
    va_start(Args, Format);

    time_t Time;
    time(&Time);
    
    int nChars = base_vsnprintf(TraceMsg, MAX_TRACE, Format, Args);    
    TraceMsg[nChars] = 0;

    printf("[ts:%d][tid:%#x][%s:%d] %s\n", 
        (unsigned short int) Time,
#ifdef _UNIX
        (int) base_getpid(),
#endif
#ifdef _WIN32
        GetCurrentThreadId(),
#endif
        m_File, 
        m_Line, 
        TraceMsg);

    va_end(Args);
}

CInternalTraceTagDesc * CInternalTrace :: GetTagInfo(long Tag, CInternalTraceTagDesc pDesc[], int nSize) {
	for (int i=0;i<nSize;i++) {
		if (pDesc[i].m_nTag == Tag) {
			return & pDesc[i];
		}
	}

	return NULL;
}

void CInternalTrace :: ShowTagInfo(long Min, long Max, CInternalTraceTagDesc pTagDesc[], int nSize) {
	for (long n = Min; n <= Max; n++) {
		CInternalTraceTagDesc * pDesc = GetTagInfo(n, pTagDesc, nSize);
		if (pDesc) {
			cout << n << ": " << pDesc->m_szDescription << endl;
		}
	}	
}

#endif
