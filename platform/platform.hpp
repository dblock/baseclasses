/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_PLATFORM_HPP
#define BASE_PLATFORM_HPP

#ifndef __cplusplus // compiles with C++ only
#error C++ Compiler Required (BASE)
#endif

#if !defined(_WIN32) && !defined(_MAC) // under UNIX define _UNIX
 #define _UNIX
#else
 #undef _UNIX
#endif

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#ifdef _WIN32
 // #define ROCKALL_HEAPS
#endif

#ifdef ROCKALL_HEAPS
 #include <RockallHeaps.hpp>
#endif

#endif
