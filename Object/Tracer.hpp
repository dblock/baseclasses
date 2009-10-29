/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TRACER_HPP
#define BASE_TRACER_HPP

#include <platform/include.hpp>

#ifdef BASE_TRACE_ENABLED

#define MAX_TRACE 256

typedef enum {
    levCrash    = 1,
    levError,
    levWarning,
    levInfo,
    levVerbose,
} CTraceLevel;

typedef enum {
    tagBuiltinMin  = 1,
    tagGeneric,
    tagSystem,
    tagMemory,
    tagSocket,
    tagHttp,
    tagLocks,
    tagDns,
    tagServer,
    tagDateTime,
    tagReserved4,
    tagBuiltinMax
} CTraceBaseTags;

extern bool s_TraceLevels[MAX_TRACE];
extern bool s_TraceTags[MAX_TRACE];

typedef struct {
	long m_nTag;
	char * m_szDescription;
} CInternalTraceTagDesc;

static CInternalTraceTagDesc s_TraceTagsDescriptions[] = {
    { tagBuiltinMin, "reserved tag" },
    { tagGeneric, "generic library functions" },
    { tagSystem, "general system functions" },
    { tagMemory, "memory allocation system requests" },
    { tagSocket, "socket system level requests" },
    { tagHttp, "http requests and responses" },
    { tagLocks, "locks, semaphores and mutexes" },
    { tagDns, "dns lookups, name resolution" },
    { tagServer, "tcp server" },
    { tagDateTime, "date/time" },
    { tagReserved4, "reserved tag" },
    { tagBuiltinMax, "reserved tag" }
};

static CInternalTraceTagDesc s_TraceLevelsDescriptions[] = {
    { levCrash, "crash" },
    { levError, "error" },
    { levWarning, "warning" },
    { levInfo, "information" },
    { levVerbose, "verbose" }
};

class CInternalTrace {
private:
    long m_Tag;
    CTraceLevel m_Level;
    char * m_File;
    int m_Line;    
    char TraceMsg[MAX_TRACE + 1];
public:
    inline static bool InternalTraceCheck(long Tag, CTraceLevel Level);
    void InternalTrace(char * Format, ...);
    CInternalTrace(long Tag, CTraceLevel Level, char * File, int Line);
	static CInternalTraceTagDesc * GetTagInfo(long Tag, CInternalTraceTagDesc pDesc[], int nSize);
	static void ShowTagInfo(long Min, long Max, CInternalTraceTagDesc pDesc[], int nSize);
    static inline CInternalTraceTagDesc * GetTraceLevelsDescriptions(void) { return (CInternalTraceTagDesc *) s_TraceLevelsDescriptions; }
    static inline CInternalTraceTagDesc * GetTraceTagsDescriptions(void) { return (CInternalTraceTagDesc *) s_TraceTagsDescriptions; }
};

inline bool CInternalTrace :: InternalTraceCheck(long Tag, CTraceLevel Level) {
    return (s_TraceTags[Tag] && s_TraceLevels[Level]);
}

#endif
#endif
