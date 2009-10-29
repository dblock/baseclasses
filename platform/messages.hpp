/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <platform/platform.hpp>

#ifndef BASE_CM_MESSAGES_HPP
#define BASE_CM_MESSAGES_HPP

typedef unsigned int CMessage;

#define CM_USER       1000
#define CM_OUT        CM_USER+1
#define CM_IN         CM_USER+2
#define CM_POST       CM_USER+3
#define CM_OUT_CLEAR  CM_USER+4
#define CM_REPLACE_PREFIX CM_USER+5

#define CM_OUT_LOCK		CM_USER+10
#define CM_OUT_UNLOCK	CM_USER+11
#define CM_OUT_FORCE	CM_USER+12

#define CM_REPLACE_POST CM_USER+20

#endif
