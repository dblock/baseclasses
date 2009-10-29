/*
  
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  original implementation Fulco Houkes - houkes@xo3.com
     © Xo3 S.A., Geneva, Switzerland    
     http://www.xo3.com, All Rights Reserved
  
  rewritten by Daniel Doubrovkine - dblock@vestris.com
 
*/

#ifndef BASE_RWMUTEX_HPP
#define BASE_RWMUTEX_HPP

#include <Object/Object.hpp>
#include <Mutex/Mutex.hpp>
#include <Mutex/Atomic.hpp>

class CRWMutex : public CObject {
protected:  
    long m_Readers;
    CMutex  m_WriteMutex;
public:
    CRWMutex (void);
    virtual ~CRWMutex (void);
    inline void StartReading (void);
    inline void StartWriting (void);
    inline void StopReading (void);
    inline void StopWriting (void);
};

inline void CRWMutex::StartReading (void) {
    m_WriteMutex.Lock();
    m_Readers++;
    m_WriteMutex.UnLock();
}

inline void CRWMutex::StartWriting (void) {
    while (1) {
        m_WriteMutex.Lock();
        if (!m_Readers)
            break;
        m_WriteMutex.UnLock();
    }
}

inline void CRWMutex::StopReading (void) {
    m_WriteMutex.Lock();
    m_Readers--;
    m_WriteMutex.UnLock();
}

inline void CRWMutex::StopWriting (void) {
    m_WriteMutex.UnLock();
}

#endif
