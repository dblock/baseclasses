/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TRACE_HPP
#define BASE_TRACE_HPP

#define BASE_TRACE_ENABLED
#ifdef BASE_TRACE_ENABLED
 #define Trace(_Tag, _Level, _Msg) \
    if (CInternalTrace::InternalTraceCheck(_Tag, _Level)) { \
        CInternalTrace InternalTrace(_Tag, _Level, __FILE__, __LINE__); \
        InternalTrace.InternalTrace _Msg; \
    }
#else
 #define Trace(_Tag, _Level, _Msg) ;
#endif

#endif
