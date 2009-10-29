/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASECLASSES_SOCKET_TYPES_HPP
#define BASECLASSES_SOCKET_TYPES_HPP

#include <platform/include.hpp>

#if defined(_WIN32) || defined(HAVE_ACCEPT_SIGNED_INT)
 #define __ACCEPT_ADDR_TYPECAST (int *)
#elif defined(HAVE_ACCEPT_UNSIGNED_INT)
 #define __ACCEPT_ADDR_TYPECAST (unsigned int *)
#endif

#ifdef __TLI_SUPPORTED
 #undef base_send
 #undef base_recv
 #define base_send t_snd
 #define base_recv t_rcv
#else
 #undef base_send
 #undef base_recv 
 #define base_send send
 #define base_recv recv
#endif
    
#endif
