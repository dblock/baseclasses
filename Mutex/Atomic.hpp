/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1999 All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    13.09.1999: added postfix and prefix operators++/--
    15.07.2000: corrected bug: Set(), NT, was using InterlockedExchangeAdd

*/

#ifndef BASE_ATOMIC_HPP
#define BASE_ATOMIC_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#ifdef _UNIX
#include <Mutex/Mutex.hpp>
#endif

class CAtomic : public CObject {    
#ifdef _UNIX
    mutable CMutex m_InterlockMutex;
#endif
    long m_Value;
public:
    CAtomic(void);
    CAtomic(long Value);
    virtual ~CAtomic(void);
    inline long Inc(long Increment = 1);
    inline long Dec(long Decrement = 1) { return Inc(-Decrement); }
    inline long Get(void) const;
    inline long Set(long Value);
    inline long operator++(int) { return Inc(); }
    inline long operator--(int) { return Dec(); }
    inline long operator++(void) { return Inc(); }
    inline long operator--(void) { return Dec(); }	
    inline long operator=(const CAtomic& Atomic);
};

inline long CAtomic::operator=(const CAtomic& Atomic) {
    if (this != &Atomic)
        return Set(Atomic.Get());
    else return Get();
}

inline long CAtomic::Get(void) const {    
    return m_Value;
}

inline long CAtomic::Set(long Value) {
#ifdef _UNIX
    m_InterlockMutex.Lock();
    m_Value = Value;
    m_InterlockMutex.UnLock();
    return Value;
#endif
#ifdef _WIN32
    InterlockedExchange(&m_Value, Value);
    return Value;
#endif
}

inline long CAtomic::Inc(long Increment) {    
#ifdef _UNIX
    m_InterlockMutex.Lock();
    m_Value += Increment;
    long lResult = m_Value;
    m_InterlockMutex.UnLock();
    return lResult;
#endif
#ifdef _WIN32
    return InterlockedExchangeAdd(&m_Value, Increment) + Increment;
#endif
}

#endif
