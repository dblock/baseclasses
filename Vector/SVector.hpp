/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
  16.08.2000: Modified method Add() to use binary search and added method FindAbs(), Hassan
  16.08.2000: Rewritten FindIndex, FindElt, FindAbs, Add, fixes, etc.  
  17.08.2000: Added operators <,>,!=,==,=, removed unuseful memset in constructor CSVector(void)
*/

#include <platform/include.hpp>

    
#ifndef ASEARCH_SVECTOR_HPP
#define ASEARCH_SVECTOR_HPP
    
#include <platform/include.hpp>

#define CSABS(X) abs(X) 
// abs() is inlined and asm ((X>0)?X:-X)
#define CSNEG(X,Y) ((X<0)?X:Y)
    
class CSVector {
private:
  property(unsigned int, Dim);
  property(unsigned int, Size);
  
  int * m_pVector;
    
  inline void Copy(const CSVector&);
  
  int FindElt(const int Element, const int Min, const int Max) const;
  int FindAbs(const int Element, const int Min, const int Max) const;
  int FindIndex(const int Element, const int Min, const int Max, bool * pInsert) const;
public:
  
  CSVector(void);
  CSVector(const CSVector&);
  ~CSVector(void);
  
  inline void SetDim(unsigned int Dim, bool bPreserve = true, bool bZero = false);
  inline void SetSize(unsigned int Size, bool bPreserve = true, bool bZero = true);
    
  ostream& operator<<(ostream&) const;
    
  // always sorted and unique
  inline void Append(const int Value);
  inline void Insert(const int Value, unsigned int Position);
  inline bool Remove(const int Value);
  inline void RemoveAt(const int Position);  
  inline bool RemoveEx(const int Value);
  inline void RemoveAll();
  inline int FindElt(const int Element) const;
  inline int FindAbs(const int Element) const;

  inline bool Add(const int Element);  

  void Add(const CSVector&);
    
  inline int& operator[](const int Index) { return m_pVector[Index]; }
  inline int operator[](const int Index) const { return m_pVector[Index]; }

  inline bool operator<(const CSVector& Vector) const;
  inline bool operator>(const CSVector& Vector) const;
  inline bool operator<=(const CSVector& Vector) const;
  inline bool operator>=(const CSVector& Vector) const;
  inline bool operator==(const CSVector& Vector) const;
  inline bool operator!=(const CSVector& Vector) const; 

  inline CSVector& operator=(const CSVector& Vector);
};

inline void CSVector::RemoveAt(const int Position) {
	assert(m_Size);
    m_Size--;	
	for (register int i=Position;i<(int)m_Size;i++)
      m_pVector[i] = m_pVector[i+1];	
}


inline bool CSVector::Remove(const int Value) {
    int Pos = m_Size ? FindElt(Value, 0, m_Size - 1) : -1;
    if (Pos >= 0)  {
        RemoveAt(Pos);		
        return true;
    } 	
    return false;
}

inline bool CSVector::RemoveEx(const int Value) {	
    int Pos = m_Size ? FindAbs(Value, 0, m_Size - 1) : -1;
    if (Pos >= 0) {
        if (m_pVector[Pos] > 0) {
			RemoveAt(Pos);
        } else {
            m_pVector[Pos] = -m_pVector[Pos];
        }		
        return true;
    }	
    return false;
}

/*
 * If the value is already present, we replace it with the new one(seems stupid but don't forget the
 * positive/negative difference)
 * If the value is not present, insert it at the right place
 */
inline bool CSVector::Add(const int Element) {
    bool bInsert = true;
    int Pos = m_Size ? FindIndex(Element, 0, (int) m_Size - 1, &bInsert) : 0;

	assert(!bInsert || (Pos >= 0));

    if (bInsert) {
        Insert(Element, Pos);
        return true;
    } else {
		assert(Pos <= 0);
        m_pVector[-Pos] = Element;
        return false;
    }	
}

inline void CSVector :: Insert(const int Value, unsigned int Position) {
  if (Position > m_Size) {
    Append(Value);
  } else {	
    SetDim(m_Size + 1, true, false);
    for (register int i = (int) m_Size; i > (int) Position; i--)
      m_pVector[i] = m_pVector[i-1];
    m_pVector[Position] = Value;
    m_Size++;	
  }  
}
    
inline void CSVector :: Append(const int Value) {
  SetDim(m_Size + 1, true, false);
  m_pVector[m_Size++] = Value;  
}
    
inline void CSVector :: SetSize(unsigned int Size, bool bPreserve, bool bZero) {
  SetDim(Size, bPreserve, bZero);
  m_Size = Size;    
}

inline void CSVector :: SetDim(unsigned int Dim, bool bPreserve, bool bZero) {
  if (m_Dim >= Dim)
    return;
  
  unsigned int TargetDim = Dim;
  
  if (TargetDim < MBYTE) {
    TargetDim = m_Dim ? (m_Dim << 1) : 32;
    while ( TargetDim < Dim)
      TargetDim <<= 1;
  }  
    
  int * pDynamic = new int[TargetDim];
  
  if (!pDynamic) {
    cout << "BASE::Fatal Error::Unable to allocate " << TargetDim << " bytes." << endl;
    assert(0);
  }
    
  if (bPreserve && m_Size)
    memcpy(pDynamic, m_pVector, m_Size * sizeof(int));
  if (bZero)
    memset(pDynamic + m_Size, 0, (TargetDim - m_Size) * sizeof(int));  
  
  if (m_pVector)
    delete[] m_pVector;
  
  m_pVector = pDynamic;
  m_Dim = TargetDim;     
}

inline void CSVector :: Copy(const CSVector& Vector) {
  SetSize(Vector.m_Size, false, false);
  memcpy(m_pVector, Vector.m_pVector, Vector.m_Size * sizeof(int));    
}

inline ostream& operator << (ostream& Stream, const CSVector& Vector) {
  return Vector.operator << (Stream);
}

inline int CSVector :: FindAbs(const int Element) const {
    return m_Size ? FindAbs(Element, 0, (int) m_Size - 1) : -1;
}

inline int CSVector :: FindElt(const int Element) const {
    return m_Size ? FindElt(Element, 0, (int) m_Size - 1) : -1;
}



inline void CSVector :: RemoveAll(void) {
    m_Size = 0;
}


inline bool CSVector::operator<(const CSVector& Vector) const {
  if (this == &Vector) return false;
  else if (m_Size < Vector.m_Size) return true;
  else if (m_Size > Vector.m_Size) return false;
  else for (register int i=0;i<(int)m_Size;i++)
    if (m_pVector[i] < Vector.m_pVector[i]) 
      return true;
  return false;
}


inline bool CSVector::operator>(const CSVector& Vector) const {
  if (this == &Vector) return false;
  else if (m_Size > Vector.m_Size) return true;
  else if (m_Size < Vector.m_Size) return false;
  else for (register int i=0;i<(int)m_Size;i++)
    if (m_pVector[i] > Vector.m_pVector[i]) 
      return true;
  return false;
}


inline bool CSVector::operator<=(const CSVector& Vector) const {
  if (this == &Vector) return true;
  else if (m_Size < Vector.m_Size) return true;
  else if (m_Size > Vector.m_Size) return false;
  else for (register int i=0;i<(int)m_Size;i++)
    if (m_pVector[i] > Vector.m_pVector[i]) 
      return false;
  return true;
}


inline bool CSVector::operator>=(const CSVector& Vector) const {
  if (this == &Vector) return true;
  else if (m_Size > Vector.m_Size) return true;
  else if (m_Size < Vector.m_Size) return false;
  else for (register int i=0;i<(int)m_Size;i++)
    if (m_pVector[i] < Vector.m_pVector[i]) 
      return false;
  return true;
}


inline bool CSVector::operator==(const CSVector& Vector) const {
  if (this == &Vector) return true;
  else if (m_Size != Vector.m_Size) return false;
  else for (register int i=0;i<(int)m_Size;i++)
    if (m_pVector[i] != Vector.m_pVector[i]) 
      return false;
  return true;
}


inline bool CSVector::operator!=(const CSVector& Vector) const {
  if (this == &Vector) return false;
  else if (m_Size != Vector.m_Size) return true;
  else for (register int i=0;i<(int)m_Size;i++)
    if (m_pVector[i] != Vector.m_pVector[i]) 
      return true;
  return false;
}

inline CSVector& CSVector :: operator=(const CSVector& Vector) {
    Copy(Vector);
    return * this;
}

#endif
