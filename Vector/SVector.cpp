/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>
#include "SVector.hpp"
    
CSVector :: CSVector(void) :
  m_Dim(0),
  m_Size(0),  
  m_pVector(NULL)
{
    
}

CSVector :: CSVector( const CSVector& Vector ) :
  m_Dim(0),
  m_Size(0),  
  m_pVector(NULL)
{
  
  Copy(Vector);
}
 
CSVector :: ~CSVector( void ) {
  if (m_pVector)
    delete[] m_pVector;
}
    

ostream& CSVector :: operator << (ostream& Stream) const {
  for (register unsigned int i=0; i < m_Size ; i++) {
    if (i)
      Stream << ' ';
    Stream << m_pVector[i];
  }
  return Stream;
}

void CSVector :: Add(const CSVector& Vector) {  
  if (!Vector.m_Size)
    return;
  
  if (!m_Size) {
    Copy(Vector);
    return;
  }

  m_Dim = m_Size + Vector.m_Size;  
  int * pVector = new int[m_Dim];
    
  unsigned int i = 0, j = 0, t = 0;
  
  do {
    if (CSABS(m_pVector[i]) == CSABS(Vector.m_pVector[j])) {      
      pVector[t++] = CSNEG(Vector.m_pVector[j], m_pVector[i]);
      i++;
      j++;
    } else if (CSABS(m_pVector[i]) < CSABS(Vector.m_pVector[j])) {
      pVector[t++] = m_pVector[i];
      i++;
    } else {
      pVector[t++] = Vector.m_pVector[j];
      j++;    
    }
  } while ((i < m_Size) && (j < Vector.m_Size));

  while (i < m_Size)
    pVector[t++] = m_pVector[i++];
  while (j < Vector.m_Size)
    pVector[t++] = Vector.m_pVector[j++];
  
  if (m_pVector)
    delete[] m_pVector;
  
  m_pVector = pVector;
  m_Size = t;  
}


int CSVector::FindAbs(const int Element, const int Min, const int Max) const {

    register int AbsElt = CSABS(Element);
    
    if ( CSABS(m_pVector[Min]) == AbsElt ) 
        return Min;
    
    if ( CSABS(m_pVector[Max]) == AbsElt ) 
        return Max;
    
    if ( Max <= Min + 1 ) 
        return -1;
    
    int Middle = Min + (Max - Min) /2 ;
    register int AbsMiddle = CSABS(m_pVector[Middle]);
    
    if ( AbsElt > AbsMiddle )
        return FindAbs(Element, Middle, Max);
    else if ( AbsElt == AbsMiddle ) 
        return Middle;
    else 
        return FindAbs(Element , Min, Middle);
}

int CSVector::FindElt(const int Element, const int Min, const int Max) const {

	if ( m_pVector[Min] == Element ) 
        return Min;
    
    if ( m_pVector[Max] == Element ) 
        return Max;
    
    if ( Max <= Min + 1 ) 
        return -1;
    
    int Middle = Min + (Max - Min) /2 ;
    
    if ( Element > m_pVector[Middle] )
        return FindElt(Element, Middle, Max);
    else if ( Element == m_pVector[Middle] ) 
        return Middle;
    else 
        return FindElt(Element, Min, Middle);
}

/*
 * If the absolute value is not present in the vector: returns the place where we should insert it
 * If the absolute value is present in the vector, return the negative position of the value
 *
 * This way, in Add, if the received value is negative, we know we must not insert the
 * value(--> add an element and shift the bigger elements) but replace a value by another one
 *
 * We MUST in any case replace the value even if it was already present, because the previous value
 * was perhaps negative and the new one positive.
 */
int CSVector :: FindIndex(const int Element, const int Min, const int Max, bool * pInsert) const {

    register int AbsElt, AbsMax, AbsMin, AbsMiddle, Middle;

    * pInsert = true;

    AbsElt = CSABS(Element);
    AbsMax = CSABS(m_pVector[Max]);
    AbsMin = CSABS(m_pVector[Min]);
    
    if ( AbsElt > AbsMax ) 
        return Max + 1;
    
    if ( AbsElt < AbsMin ) 
        return Min;
    
    if (Min == Max - 1) {
        
        if ( AbsElt == AbsMax ) {
            * pInsert = false;
            return -Max;
        }

        if ( AbsElt == AbsMin ) {
            * pInsert = false;
            return -Min;
        }

        return Max;
    }
    
    Middle = Min + (Max - Min) / 2;
    AbsMiddle = CSABS(m_pVector[Middle]);
    
    if ( AbsElt == AbsMiddle ) {
        * pInsert = false;
        return - Middle;
    } else if ( AbsElt < AbsMiddle ) {
        return FindIndex(Element, Min, Middle, pInsert);
    } else {
        return FindIndex(Element, Middle, Max, pInsert);  
	}
}

