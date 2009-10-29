/*
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com


  Revision history:

  23.02.2000: using a stack allocated block for small vectors
  05.04.2000: Added FindSortedElt(T,min,max): binary search for an element, Hassan
  13.05.2000: handler/container implementation switch

*/

#ifndef BASE_VECTOR_HPP
#define BASE_VECTOR_HPP

#ifdef _WIN32
#pragma warning( disable : 4700)  
#endif

#include <platform/include.hpp>
#include <Object/Object.hpp>

#ifdef _WIN32
#pragma warning( disable : 4700)  
#endif

template <class T>
class CVector : public CObject {
private:
  BASE_GUARD(m_StackGuard);
private:
  readonly_property(unsigned long, Size); // real number of elements
  readonly_property(unsigned long, Dim); // dimension of the allocated array
  readonly_copy_property(T *, Data);   
  BASE_GUARD(m_PropGuard);
public:
  inline void SetSize(unsigned long); // set the real number of elements
  inline void SetDim(unsigned long, bool bPreserve = true); // attempt to grow the allocated array
  inline void SetAll(const T& Value);
  CVector(unsigned long Dimension = 0, const bool Initialize = false);
  CVector(const CVector<T>&);
  CVector<T>& operator=(const CVector<T>&);
  virtual ~CVector(void);
  /* streaming */
  inline ostream& operator<<(ostream& Stream) const;
  /* concatenation */
  inline void Add(const T&);
  inline void operator+=(const T& Elt) { Add(Elt); }
  inline long FindSortedElt(const T& Elt) const { return m_Size?FindSortedElt(Elt, 0, GetSize() - 1):-1; }
  long FindSortedElt(const T&, const long, const long) const;
  long AddSortedUnique(const T&);
  long FindSortedIndex(const T&, const long, const long) const;
  void Remove(const T&);
  inline void operator-=(const T& Elt) { Remove(Elt); }
  void Add(const CVector<T>&);
  inline void operator+=(const CVector<T>& Vector) { Add(Vector); }
  void RemoveAt(const unsigned long);
  void RemoveAt(unsigned long, unsigned long);
  void InsertAt(const unsigned long, const T&);
  void InsertAt(const unsigned long, const CVector<T>&);
  /* modification, access */
  inline const T& GetAt(const unsigned long Index) const { _S_DEBUG(assert(Index < m_Size)); return m_Data[Index]; }    
  inline void SetAt(const unsigned long Index, const T& Element) { if (Index >= m_Size) SetSize(Index); m_Data[Index] = Element; }
  inline const T& operator[](const unsigned long Index) const { _S_DEBUG(assert(Index < m_Size)); return m_Data[Index]; }
  inline T& operator[](const unsigned long Index) { _S_DEBUG(assert(Index < m_Size)); return m_Data[Index]; }
  void RemoveAll(void);
  inline long Find(const T& Element) const;
  inline bool Contains(const T& Element) const { return (Find(Element) != -1); }
  /* comparison */
  inline bool operator<(const CVector<T>& Vector) const;
  inline bool operator>(const CVector<T>& Vector) const;
  inline bool operator<=(const CVector<T>& Vector) const;
  inline bool operator>=(const CVector<T>& Vector) const;
  inline bool operator==(const CVector<T>& Vector) const;
  inline bool operator!=(const CVector<T>& Vector) const;         
  /* sorting */
  inline void QuickSort(void) { if (m_Size) QuickSort(0, m_Size - 1); }
  void QuickSortUnique(void);
private:
  inline void Assign(const unsigned long Index, const T& Element);
  void QuickSort(const long l, const long r);  
};

template <class T>
inline void CVector<T>::Add(const T& Element) {
  SetDim(m_Size + 1);
  Assign(m_Size, Element);
  m_Size++;
}

template <class T>
inline ostream& CVector<T>::operator<<(ostream& Stream) const {
  for (register unsigned long i=0; i < m_Size; i++)
    Stream << m_Data[i] << endl;
  return Stream;
}

template <class T>
inline ostream& operator<<(ostream& Stream, const CVector<T>& Vector) {
  return Vector.operator<<(Stream);
}

/* comparison */

template <class T>
inline bool CVector<T>::operator<(const CVector<T>& Vector) const {
  if (this == &Vector) return false;
  else if (m_Size < Vector.m_Size) return true;
  else if (m_Size > Vector.m_Size) return false;
  else for (register unsigned long i=0; i < m_Size; i++)
    if (m_Data[i] < Vector.m_Data[i]) return true;
  return false;
}

template <class T>
inline bool CVector<T>::operator>(const CVector<T>& Vector) const {
  if (this == &Vector) return false;
  else if (m_Size > Vector.m_Size) return true;
  else if (m_Size < Vector.m_Size) return false;
  else for (register unsigned long i=0; i < m_Size; i++)
    if (m_Data[i] > Vector.m_Data[i]) return true;
  return false;
}

template <class T>
inline bool CVector<T>::operator<=(const CVector<T>& Vector) const {
  if (this == &Vector) return true;
  else if (m_Size < Vector.m_Size) return true;
  else if (m_Size > Vector.m_Size) return false;
  else for (register unsigned long i = 0; i < m_Size; i++)
    if (m_Data[i] > Vector.m_Data[i]) return false;
  return true;
}

template <class T>
inline bool CVector<T>::operator>=(const CVector<T>& Vector) const {
  if (this == &Vector) return true;
  else if (m_Size > Vector.m_Size) return true;
  else if (m_Size < Vector.m_Size) return false;
  else for (register unsigned long i = 0; i < m_Size; i++)
    if (m_Data[i] < Vector.m_Data[i]) return false;
  return true;
}

template <class T>
inline bool CVector<T>::operator==(const CVector<T>& Vector) const {
  if (this == &Vector) return true;
  else if (m_Size != Vector.m_Size) return false;
  else for (register unsigned long i = 0; i < m_Size; i++)
    if (m_Data[i] != Vector.m_Data[i]) return false;
  return true;
}

template <class T>
inline bool CVector<T>::operator!=(const CVector<T>& Vector) const {
  if (this == &Vector) return false;
  else if (m_Size != Vector.m_Size) return true;
  else for (register unsigned long i = 0; i < m_Size; i++)
    if (m_Data[i] != Vector.m_Data[i]) return true;
  return false;
}

template <class T>
inline long CVector<T>::Find(const T& Element) const {
  for (register unsigned long i = 0; i < m_Size; i++)
    if (Element == m_Data[i]) return i;
  return -1;
}

#ifdef BASE_DEBUG
#define VECTOR_IC_GUARD_INIT                    \
  : BASE_GUARD_INIT(m_StackGuard)               \
  , BASE_GUARD_INIT(m_PropGuard)
#else
#define VECTOR_IC_GUARD_INIT
#endif

/* constructors / destructors */

template <class T>
CVector<T>::CVector(unsigned long Dimension, 
                    const bool Initialize)  
  VECTOR_IC_GUARD_INIT
{
  m_Size = 0;
  m_Dim = 0;
  m_Data = NULL;
  if (Dimension) {
    if (Initialize) 
      SetSize(Dimension); 
    else SetDim(Dimension, false);
  }
}

template <class T>
CVector<T>::~CVector(void) {
  BASE_GUARD_CHECK(m_StackGuard);
  BASE_GUARD_CHECK(m_PropGuard);
  RemoveAll();
}

template <class T>
CVector<T>& CVector<T>::operator=(const CVector<T>& Vector) {
  if (&Vector != this) {
    // VECTOR_BASECLASS::operator=(Vector);
    SetDim(Vector.m_Size, false);
    for (register unsigned long i = 0; i < Vector.m_Size; i++) {
      Assign(i, Vector.m_Data[i]);
    }
    m_Size = Vector.m_Size;
  }
  return * this;
}

template <class T>
CVector<T>::CVector(const CVector<T>& Vector)
  VECTOR_IC_GUARD_INIT
{
  m_Size = 0;  
  m_Dim = 0;
  m_Data = NULL;
  operator=(Vector);
}

/* size and dimension */

template <class T>
inline void CVector<T>::SetSize(unsigned long Size) {
  SetDim(Size);
  for (register unsigned long i = m_Size; i < Size; i++) {
    new((void *) & m_Data[i]) T;
  }
  m_Size = Size;
}

template <class T>
inline void CVector<T>::SetDim(unsigned long DesiredDim, bool bPreserve) {

  // catch growth of negative values casted by mistake
  _S_DEBUG(assert(DesiredDim < GBYTE));
  
  if (DesiredDim > m_Dim) {
    
    unsigned long Dim = BASE_MAX(DesiredDim, 11);
    if ((Dim < MBYTE) && (m_Dim)) {
      Dim = m_Dim << 1;
      while ( Dim < DesiredDim)
        Dim <<= 1;
    }
    
    T * NewData = (T *) new BYTE[sizeof(T) * Dim];

    if (bPreserve || m_Size) {
      for (register unsigned long i = 0; i < m_Size; i++) {
	new((void *) & NewData[i]) T;
	if (bPreserve) {	  
	  NewData[i] = m_Data[i];
	}
	m_Data[i].~T();
      }
    }
    
    if (m_Data) {
      delete[] (BYTE *) m_Data;
    }
    
    m_Data = NewData;
    m_Dim = Dim;
  }
}

/* deletion */
template <class T>
void CVector<T>::Remove(const T& Element) {
  T * Current = m_Data; 
  T * Ahead = m_Data;
  T * Tail = m_Data + m_Size;
  while (Current < Tail) {
    if (Element == (* Current)) {
      m_Size--;
    } else {
      if (Ahead != Current) {
	* Ahead = * Current;
      }
      Ahead ++;
    }
    Current ++;
  }
  while (Ahead < Tail) {
    Ahead->~T();
    Ahead++;
  }
}
 
template <class T>
void CVector<T>::RemoveAt(const unsigned long Index) {
  _S_DEBUG(assert(Index < m_Size));

  if (Index >= m_Size)
    return;

  m_Size--;

  for (unsigned long i = Index; i < m_Size; i++) {
    m_Data[i] = m_Data[i+1];
  }
  
  m_Data[m_Size].~T();
}

template <class T>
void CVector<T>::RemoveAt(unsigned long Index, unsigned long Count) {
  register unsigned long i;
  
  if (Index >= m_Size)
    return;
  
  if (Index + Count >= m_Size)
    Count = m_Size - Index;
  
  for (i = Index; i < m_Size - Count; i++) {
    m_Data[i] = m_Data[i + Count];
  }
  
  for (i = m_Size - Count; i < m_Size; i++) {
    m_Data[i].~T();
  }
  
  m_Size -= Count;
}

template <class T>
void CVector<T>::InsertAt(const unsigned long Index, const T& Element) {
  if (Index < m_Size) {
    SetDim(m_Size + 1);
    new((void *) & m_Data[m_Size]) T();
    for (register unsigned long i = m_Size; i > Index; i--) {
      m_Data[i] = m_Data[i-1];
    }
    m_Data[Index] = Element;
    m_Size++;
  } else {
    Add(Element);
  }
}

template <class T>
inline void CVector<T> :: Assign(const unsigned long Index, const T& Element) {
  assert(Index < m_Dim);
  if (Index < m_Size) {
    m_Data[Index] = Element;
  } else {
    new((void *) & m_Data[Index]) T();
    m_Data[Index] = Element;
  }
}

template <class T>
void CVector<T>::InsertAt(const unsigned long Index, const CVector<T>& Vector) {	
  SetDim(m_Size + Vector.GetSize());
  register unsigned long i;
  if (m_Size) {
    for (i = m_Size; i > Index; i--) {
      Assign(i + Vector.GetSize() - 1, m_Data[i - 1]);
    }
  }
  for (i = 0; i < Vector.GetSize(); i++) {
    Assign(i + Index, Vector[i]);
  }
  m_Size += Vector.GetSize();
}

template <class T>
void CVector<T>::Add(const CVector<T>& Vector) {
  // WARNING! appending a vector to itself should work
  SetDim(m_Size + Vector.m_Size);
  for (unsigned long i = 0; i < Vector.m_Size; i++) {
    new((void *) & m_Data[i + m_Size]) T();
    m_Data[i + m_Size] = Vector.m_Data[i];
  }
  m_Size += Vector.m_Size;
}

template <class T>
void CVector<T>::QuickSortUnique(void) {
  
  if (! m_Size)
    return;
  
  QuickSort();

  register unsigned long j = 0;
  for (register unsigned long i = 1; i < m_Size; i++) {
    if (m_Data[i] != m_Data[j]) {
      j++;
      if (j != i) {
	m_Data[j] = m_Data[i];
      }
    }
  }
  
  while (m_Size > j + 1) {
    m_Size--;
    m_Data[m_Size].~T();    
  }
}

template <class T>
void CVector<T>::QuickSort(const long l, const long r) {
  long i = l;
  long j = r;
  T pivot = m_Data[(l+r)/2];
  while (i<=j) {
    while((m_Data[i]<pivot)&&(i <= r)) {
      i++;
    }
    while((m_Data[j]>pivot)&&(j >= l)) {
      j--;
    }
    
    if (i <= j){
      T t = m_Data[i];
      m_Data[i] = m_Data[j];
      m_Data[j] = t;
      i++;
      j--;
    }
  }
  if (l < j) QuickSort(l, j);
  if (i < r) QuickSort(i, r);
}

template <class T> 
long CVector<T>::FindSortedElt(const T& Element, const long Min, const long Max) const
{
  if (m_Data[Min] == Element) return Min;
  if (m_Data[Max] == Element) return Max;
  if (Max <= Min+1) return -1;
  register long Middle = Min + (Max -Min) /2 ;
    
  if (Element > m_Data[Middle]) return FindSortedElt(Element, Middle, Max);
  if (Element == m_Data[Middle]) return Middle;
  if (Element < m_Data[Middle]) return FindSortedElt(Element , Min, Middle);
    
  return -1;
}

template <class T>
long CVector<T>::FindSortedIndex(const T& Element, const long Min, const long Max) const {
  if (Element>m_Data[Max]) return Max + 1;
  if (Element<m_Data[Min]) return Min;
  if (Min == Max - 1) {
    if (Element == m_Data[Max]) return -1;
    if (Element == m_Data[Min]) return -1;
    return Max;
  }
  register long Middle = Min + (Max - Min) / 2;
  if (Element == m_Data[Middle]) return -1;
  if (Element < m_Data[Middle]) return FindSortedIndex(Element,Min,Middle);
  else if (Element>m_Data[Middle]) return FindSortedIndex(Element,Middle,Max);
  else return Middle;  
}

template <class T>
long CVector<T>::AddSortedUnique(const T& Element) {
  if (! m_Size) {
    Add(Element);
    return 0;
  } else {
    long Pos = FindSortedIndex(Element, 0, GetSize() - 1);
    if (Pos >= 0) InsertAt(Pos, Element);
    return Pos;
  }
}

template <class T>
void CVector<T>::RemoveAll(void) {
  if (m_Data && m_Size) {
    for (register unsigned long i = 0; i < m_Size; i++) {
      m_Data[i].~T();
    }
    delete[] (BYTE *) m_Data;
    m_Data = NULL;
  }
  m_Dim = 0;
  m_Size = 0;
}

template <class T>
void CVector<T>::SetAll(const T& Value) {
  for (register unsigned long i = 0; i < m_Size; i++) {
    m_Data[i] = Value;
  }
}

typedef CVector<int> CVectorInt;
#endif
