/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_INT_VECTOR_HPP
#define BASE_INT_VECTOR_HPP

#include <platform/include.hpp>
#include <Vector/Vector.hpp>

#ifdef BASE_DEBUG
 #define INTVECTOR_GUARD 1
 #define INTVECTOR_GUARD_VALUE 0xDEAD
#endif
 
class CIntVector : public CObject {    
    copy_property(int *, CArray);
    property(int, CArrayMemorySize);
    property(int, CArrayMemoryFill);
    property(int, CArrayRealSize);
public:
    static const int HByte;
    static const int XByte;
    /* constructors */
    CIntVector(void);
    CIntVector(const CIntVector&);
    CIntVector& operator=(const CIntVector&);
    virtual ~CIntVector(void);
    /* copy retrieval */
    int * GetVector(void) const;
    void AddInterval(int First, int Last);
    void AppendVector(const CIntVector& Vector);
    inline void operator+=(int Value) { InsertElt(Value); }
    inline void operator+=(const CIntVector& Vector) { AppendVector(Vector); }
    inline void AddElt(int Value) { InsertElt(Value); }    
    bool InsertElt(int Value);
    bool DeleteEltAt(int Index);
    bool DeleteElt(int Value);
    bool Contains(int Value) const;
    int GetEltAt(int Index) const;
    inline int operator[](int Index) const { return GetEltAt(Index); }
    inline int GetSize(void) const { return m_CArrayRealSize; }
    bool operator==(const CIntVector&) const;
    bool operator>(const CIntVector&) const;
    bool operator<(const CIntVector&) const;
    bool operator>=(const CIntVector&) const;
    bool operator<=(const CIntVector&) const;
    bool operator!=(const CIntVector&) const;
    ostream& operator<<(ostream&) const;
    istream& operator>>(istream&);
    inline void RemoveAll(void);
    int FindElt(int) const;
    inline float GetMemorySize(void) const;
    void _AppendElt(int Value);
    void _AppendInt(int First, int Last);
    void Write(FILE *) const;    
    CVector<int>& AppendTo(CVector<int>& Target) const;
    
private:
    void RemoveFromGroup(int i, int, int, int);
    void RemoveSingle(int i);
    bool CCVPartOf(int, int);
    void CCVResize(int NewSize);
    inline int * Allocate(size_t Elements);
    inline void Free(int * Store);
};

inline float CIntVector::GetMemorySize(void) const { 
    return (float)((float)sizeof(CIntVector) + (float)m_CArrayMemorySize);
}

inline void CIntVector::RemoveAll(void) {    
    m_CArrayMemoryFill = 0;
    m_CArrayRealSize = 0;
}

inline int * CIntVector::Allocate(size_t Elements) {
  #ifdef INTVECTOR_GUARD
    Elements+=2;
  #endif
    int * pMem = (int *) new int[Elements];
  #ifdef INTVECTOR_GUARD
    pMem[0] = INTVECTOR_GUARD_VALUE;
    pMem[Elements - 1] = INTVECTOR_GUARD_VALUE;
    pMem++;
  #endif
    return pMem; 
}

inline void CIntVector::Free(int * Store) {
  #ifdef INTVECTOR_GUARD
    Store--;
    // cout << ":" << INTVECTOR_GUARD_VALUE << ":" << Store[0] << " <= guard {" << * this << "} guard => " << Store[m_CArrayMemorySize + 1] << endl;
    assert(Store[0] == INTVECTOR_GUARD_VALUE);
    assert(Store[m_CArrayMemorySize + 1] == INTVECTOR_GUARD_VALUE);
  #endif
    delete[] Store;
    m_CArrayMemorySize = 0;
} 

#endif
