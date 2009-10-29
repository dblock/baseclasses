/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Serge Huber - huber@xo3.com
    and Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_OBJECT_HPP
#define BASE_OBJECT_HPP
    
#include <platform/include.hpp>
#include <platform/debug.hpp>
#include <Object/Handler.hpp>

class CObject {	
    BASE_GUARD(m_ClassGuard);
public:
    inline CObject(void) BASE_GUARD_INIT_FN(m_ClassGuard) { 
        
    }
    
    inline CObject(const CObject& /* Object */) BASE_GUARD_INIT_FN(m_ClassGuard) { 
        
    }
    
    virtual ~CObject(void) { 
        BASE_GUARD_CHECK(m_ClassGuard); 
    }
    
    virtual CObject& operator=(const CObject&) {
        return * this; 
    }
    
    virtual bool operator<(const CObject& Object) const { if (&Object < this) return true; else return false; }
    virtual bool operator>(const CObject& Object) const { if (&Object > this) return true; else return false; }
    virtual bool operator<=(const CObject& Object) const { if (&Object <= this) return true; else return false; }
    virtual bool operator>=(const CObject& Object) const { if (&Object >= this) return true; else return false; }
    virtual bool operator!=(const CObject& Object) const { if (&Object != this) return true; else return false; }
    virtual bool operator==(const CObject &Object) const { if (&Object == this) return true; else return false; }
    
    virtual ostream& operator<<(ostream& Stream) const { return Stream; }
    friend ostream& operator<<(ostream&, const CObject&);
    virtual istream& operator>>(istream& Stream) { return Stream; }
    friend istream& operator>>(istream&, CObject&);
    friend ostream& operator<<(ostream& Stream, const CObject * Object);
    friend istream& operator>>(istream& Stream, CObject * Object);
#ifdef _WIN32
    static void ShowLastError(void) { CHandler::ShowLastError(); }
#endif
    
    static void * __object_new(size_t stAllocateBlock);
    static void __object_delete(void * pvMem);
       
};

inline ostream& operator<<(ostream& Stream, const CObject& Object) { return Object.operator<<(Stream); }
inline istream& operator>>(istream& Stream, CObject& Object) { return Object.operator>>(Stream); }

inline ostream& operator<<(ostream& Stream, const CObject * Object) { return Object->operator<<(Stream); }
inline istream& operator>>(istream& Stream, CObject * Object) { return Object->operator>>(Stream); }

#ifdef _UNIX
void * operator new( size_t stAllocateBlock );
void * operator new[](size_t stAllocateBlock);
void operator delete[](void * pvMem);
void operator delete(void * pvMem);
#else
inline void * operator new( size_t stAllocateBlock ) { return CObject :: __object_new(stAllocateBlock); }
inline void * operator new[]( size_t stAllocateBlock ) { return CObject :: __object_new(stAllocateBlock); }
inline void operator delete[](void * pvMem) { CObject :: __object_delete(pvMem); }
inline void operator delete(void * pvMem) { CObject :: __object_delete(pvMem); }
#endif
   
#endif
