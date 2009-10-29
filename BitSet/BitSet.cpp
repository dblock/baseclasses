/*
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com


  02.09.1999: (Serge Huber - huber@xo3.com) Fixed memcpy and memset bug that weren't copying
  enough data in Grow() and operator=()
  
  
*/

#include <baseclasses.hpp>
#include "BitSet.hpp"

CBitSet::CBitSet(void) {
    m_BitField = 0;
    m_BitFieldSize = 0;
    m_BitFieldCount = 0;    
}

CBitSet::CBitSet(const CBitSet& BitSet) : CObject() {
    m_BitField = 0;
    m_BitFieldSize = 0;
    m_BitFieldCount = 0;    
    operator=(BitSet);
}

CBitSet::~CBitSet(void) {
    if (m_BitField) {
        delete[] m_BitField;
    }
}

ostream& CBitSet::operator<<(ostream& Stream) const {
    char BitBlock[BITSET_INTBIT_SIZE + 1];
    BitBlock[BITSET_INTBIT_SIZE] = 0;
    unsigned int i;
    
    for (i=0;i<m_BitFieldCount;i++) {
        if ((i) && ((i%BITSET_INTBIT_SIZE) == 0)) {
            Stream << BitBlock << "|";
            Stream.flush();
        }
        BitBlock[i % BITSET_INTBIT_SIZE] = GetBit(i) + '0';
    }
    
    if (i%BITSET_INTBIT_SIZE)
        BitBlock[i%BITSET_INTBIT_SIZE] = 0;
    
    Stream << BitBlock;
    Stream.flush();
    
    return Stream;
}

CBitSet& CBitSet::operator=(const CBitSet& BitSet) {
    Grow(BitSet.m_BitFieldSize);
    memcpy(m_BitField, BitSet.m_BitField, BitSet.m_BitFieldSize*sizeof(int));
    m_BitFieldCount = BitSet.m_BitFieldCount;
    return * this;
}

