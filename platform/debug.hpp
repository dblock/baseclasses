/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_DEBUG_HPP
#define BASE_DEBUG_HPP

#ifdef _DEBUG
 #define BASE_DEBUG
#endif

#ifdef BASE_DEBUG
 #define _S_DEBUG(x) x
#else
 #define _S_DEBUG(x) /* x */;
#endif // _DEBUG

// guards
#ifdef BASE_DEBUG
 #define BASE_GUARD_DEADBEEF 0xDEADBEEF
 #define BASE_GUARD(__Name) const unsigned long __Name
 #define BASE_GUARD_CHECK(__Name) {                                             \
  if (__Name != BASE_GUARD_DEADBEEF) {                                          \
    cout << "Guard damage at " <<  __LINE__ << "(" << __FILE__ << ")" << endl;  \
    CHandler :: Terminate(-1);                                                  \
  }                                                                             \
  assert(__Name == BASE_GUARD_DEADBEEF);                                        \
 }
 #define BASE_GUARD_INIT(__Name) __Name(BASE_GUARD_DEADBEEF)
 #define BASE_GUARD_INIT_FN(__Name) : __Name(BASE_GUARD_DEADBEEF)
 #define BASE_GUARD_INIT_CN(__Name) , __Name(BASE_GUARD_DEADBEEF)
#else
 #define BASE_GUARD(__Name)
 #define BASE_GUARD_CHECK(__Name)
 #define BASE_GUARD_INIT_FN(__Name)
 #define BASE_GUARD_INIT_CN(__Name)
#endif
 
#endif
