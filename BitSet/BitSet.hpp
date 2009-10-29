/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_BIT_SET_HPP
#define BASE_BIT_SET_HPP

#include <platform/platform.hpp>
#include <Object/Object.hpp>

#define BITSET_INTBIT_SIZE 8
#define BITSET_INTBIT_TYPE unsigned char

class CBitSet : public CObject {
    unsigned int m_BitFieldCount;
    unsigned int m_BitFieldSize;
    BITSET_INTBIT_TYPE * m_BitField;
public:
    void Grow(unsigned int);
    inline int GetSize(void) const { return m_BitFieldCount; }
    inline int GetDim(void) const { return m_BitFieldSize * BITSET_INTBIT_SIZE; }
    CBitSet(void);
    CBitSet(const CBitSet&);
    CBitSet& operator=(const CBitSet&);
    virtual ~CBitSet(void);
    inline void RemoveAll(void);
    inline void SetBit(unsigned int Index);
    inline void UnSetBit(unsigned int Index);
    inline int GetBit(unsigned int Index) const;    
    int GetFirstUnset(bool bSet);
    inline int GetFirstSet(void) const;
    inline int GetLastSet(void) const;
    inline int GetNextSet(unsigned int Bit) const;
    inline int GetPrevSet(unsigned int Bit) const;
    inline int CountBits(bool bSet) const;
    ostream& operator<<(ostream& Stream) const;
    friend ostream& operator<<(ostream&, const CBitSet&);
};

inline ostream& operator<<(ostream& Stream, const CBitSet& BitSet) {
    return BitSet.operator<<(Stream);
}

inline void CBitSet::Grow(unsigned int Bits) {
    unsigned int DesiredSize = (Bits / BITSET_INTBIT_SIZE) + 2;

    if (DesiredSize > m_BitFieldSize) {

        // used by swap, need to pull out of it before new can be used
        BITSET_INTBIT_TYPE * NewField = (BITSET_INTBIT_TYPE *) new BITSET_INTBIT_TYPE[DesiredSize];
        memset(NewField, 0, DesiredSize * sizeof(BITSET_INTBIT_TYPE));
        if (m_BitField) {
            memcpy(NewField, m_BitField, m_BitFieldSize * sizeof(BITSET_INTBIT_TYPE));
            delete[] m_BitField;
        }
        m_BitField = NewField;
        m_BitFieldSize = DesiredSize;
    }

    if (Bits >= m_BitFieldCount) 
        m_BitFieldCount = (Bits + 1);
}

inline void CBitSet::SetBit(unsigned int Index) {
    Grow(Index);
    m_BitField[Index / BITSET_INTBIT_SIZE] |= (1 << (BITSET_INTBIT_SIZE - (Index%BITSET_INTBIT_SIZE) - 1));
}

inline void CBitSet::UnSetBit(unsigned int Index) {
    Grow(Index);
    m_BitField[Index / BITSET_INTBIT_SIZE] &= ~(1 << (BITSET_INTBIT_SIZE - (Index%BITSET_INTBIT_SIZE) - 1));
}

inline int CBitSet::GetBit(unsigned int Index) const {
    if (Index >= m_BitFieldCount)
        return 0;
    return ((m_BitField[Index / BITSET_INTBIT_SIZE] & (1<<(BITSET_INTBIT_SIZE - (Index%BITSET_INTBIT_SIZE) - 1))) > 0);
}

void CBitSet::RemoveAll(void) {
    if (m_BitFieldSize) {
        if (m_BitField) {
            delete[] m_BitField;
        }
        m_BitFieldSize = 0;
        m_BitFieldCount = 0;
        m_BitField = 0;
    }
}

inline int CBitSet::GetFirstUnset(bool bSet) {      
    for (unsigned Index = 0; Index < m_BitFieldSize; Index++) {
        // all bits set?
        if (m_BitField[Index] != 0xFF)
            for (register unsigned int Bit = 0; Bit < BITSET_INTBIT_SIZE; Bit++) {
                if ((m_BitField[Index] & (1 << (BITSET_INTBIT_SIZE - Bit - 1))) == 0) {
                    if (bSet)
                        m_BitField[Index] |= (1 << (BITSET_INTBIT_SIZE - Bit - 1));
                    int BitIndex = Index * BITSET_INTBIT_SIZE + Bit;
                    if (BitIndex >= (int) m_BitFieldCount)
                        m_BitFieldCount = BitIndex + 1;
                    return BitIndex;
                }
            }
    }
    
    int NewBitIndex = m_BitFieldCount;
    Grow(NewBitIndex + 1);
    if (bSet)
        m_BitField[NewBitIndex / BITSET_INTBIT_SIZE] |= (1 << (BITSET_INTBIT_SIZE - 1));  
    return NewBitIndex;    
}

inline int CBitSet::CountBits(bool bSet) const {
    int Bits = 0;    
    for (unsigned int Index = 0; Index < m_BitFieldSize; Index++) {
        if (m_BitField[Index] == 0xFF) {
            if (bSet)
                Bits += 8;
        } else for (register unsigned int Bit = 0; Bit < BITSET_INTBIT_SIZE; Bit++) {
            if (m_BitField[Index] & (1 << (BITSET_INTBIT_SIZE - Bit - 1))) {
                if (bSet) Bits ++;
            } else if (!bSet) Bits++;
        }
    }
    return Bits;    
}

inline int CBitSet::GetFirstSet(void) const {  
    for (unsigned int Index = 0; Index < m_BitFieldSize; Index++) {
        // no bits set?
        if (m_BitField[Index] != 0)
            for (register unsigned int Bit = 0; Bit < BITSET_INTBIT_SIZE; Bit++) {
                if ((m_BitField[Index] & (1 << (BITSET_INTBIT_SIZE - Bit - 1))) != 0) {
                    return Index * BITSET_INTBIT_SIZE + Bit;
                }
            }
    }
    return -1;
}

inline int CBitSet::GetLastSet(void) const {  
    for (int Index = m_BitFieldSize - 1; Index >= 0; Index--) {
        // no bits set?
        if (m_BitField[Index] != 0)
            for (register int Bit = BITSET_INTBIT_SIZE - 1; Bit >= 0; Bit--) {
                if ((m_BitField[Index] & (1 << (BITSET_INTBIT_SIZE - Bit - 1))) != 0) {
                    return Index * BITSET_INTBIT_SIZE + Bit;
                }
            }
    }
    return -1;
}

inline int CBitSet::GetNextSet(unsigned int mBit) const {  
    for (register unsigned int i = mBit + 1; i < m_BitFieldCount; i++) {
        if (GetBit(i)) {
            return i;
        }
    }
    return -1;
}

inline int CBitSet::GetPrevSet(unsigned int mBit) const {  
    for (register int i = mBit - 1; i >= 0; i--) {
        if (GetBit(i)) {
            return i;
        }
    }
    return -1;
}

#endif
