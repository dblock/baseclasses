/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

  09.09.1999: fixed interation m_CArrayMemoryFill-iCollapseCounter in RemoveFromGroup

*/

#include <baseclasses.hpp>
#include "IntVector.hpp"

const int CIntVector::HByte = (1<<31);
const int CIntVector::XByte = ~HByte;

bool CIntVector::operator==(const CIntVector& Vector) const {
    if (m_CArrayMemoryFill != Vector.m_CArrayMemoryFill) return false;
    for (register int i=0;i<m_CArrayMemoryFill;i++)
        if (m_CArray[i] != Vector.m_CArray[i]) return false;
        return true;
}

bool CIntVector::operator>=(const CIntVector& Vector) const {
    if (m_CArrayRealSize >= Vector.m_CArrayRealSize) return true;
    else return false;
}

bool CIntVector::operator<=(const CIntVector& Vector) const {
    if (m_CArrayRealSize <= Vector.m_CArrayRealSize) return true;
    else return false;
}

bool CIntVector::operator>(const CIntVector& Vector) const {
    if (m_CArrayRealSize > Vector.m_CArrayRealSize) return true;
    else return false;
}

bool CIntVector::operator<(const CIntVector& Vector) const {
    if (m_CArrayRealSize < Vector.m_CArrayRealSize) return true;
    else return false;
}

bool CIntVector::operator!=(const CIntVector& Vector) const {
    return !(operator==(Vector));
}

CIntVector::CIntVector(const CIntVector& Vector) {
    m_CArray = NULL;
    m_CArrayRealSize = 0;
    m_CArrayMemorySize = 0;
    m_CArrayMemoryFill = 0;
    operator=(Vector);
}

CIntVector& CIntVector::operator=(const CIntVector& Vector) {
    if ((&Vector) == this) 
        return (* this);
    CCVResize(Vector.m_CArrayMemoryFill);
    if (Vector.m_CArrayMemoryFill) {
        memcpy(m_CArray, Vector.m_CArray, Vector.m_CArrayMemoryFill * sizeof(int));
        m_CArrayMemoryFill = Vector.m_CArrayMemoryFill;
        m_CArrayRealSize = Vector.m_CArrayRealSize;
    } else {
        m_CArrayMemoryFill = 0;
        m_CArrayRealSize = 0;
    }
    return (* this);
}

CIntVector::~CIntVector(void) {
    if (m_CArray) 
        Free(m_CArray);
}

CIntVector::CIntVector(void) {
    m_CArrayMemorySize = 0;
    m_CArrayMemoryFill = 0;
    m_CArrayRealSize = 0;
    m_CArray = NULL;
}

bool CIntVector::CCVPartOf(int Index, int Value) {
    if (Index < m_CArrayMemoryFill) {
        if ((m_CArray[Index]&HByte) && (Index < (m_CArrayMemoryFill - 1))) {
            if ((m_CArray[Index+1] <= Value)&&((m_CArray[Index+1]+(m_CArray[Index]&XByte)) >= Value)) return true;
            else return false;
        } else return (m_CArray[Index] == Value);
    } else return false;
}

void CIntVector::RemoveFromGroup(int i, int Value, int iEltMin, int iEltCount) {
    // element is inside the group
    // split the group
    if ((Value == iEltMin)||(Value == (iEltMin + iEltCount - 1))) {
        // leftmost/rightmost element
        if (Value == iEltMin) m_CArray[i+1]++;
        m_CArray[i]--;
        //cout << "unique counter: [" << (m_CArray[i]&XByte) << "]" << endl;
        if ((m_CArray[i]&XByte) == 1) {
            // merge into a single element
            for (register int j=i;j<(m_CArrayMemoryFill-1);j++)
                m_CArray[j] = m_CArray[j+1];
            m_CArrayMemoryFill--;
        }
        m_CArrayRealSize--;
    } else {
        // somewhere middle element
        // idea: separte into two lists and then collapse lists if needed
        // add a new empty list
        CCVResize(m_CArrayMemoryFill + 2);
        for (register int j=m_CArrayMemoryFill-1;j>=(i+2);j--)
            m_CArray[j+2] = m_CArray[j];
        m_CArrayMemoryFill+=2;
        // set the right list
        // cout << "Right list counter:[" << ((m_CArray[i] - (Index - iCounter + 1))&XByte) << "]" << endl;
        // cout << "Left list counter:[" << (Index - iCounter) << "]" << endl;
        int iCollapseCounter = 0;
        
        m_CArray[i+2] = m_CArray[i] - (Value - iEltMin + 1); // counter (right)
        m_CArray[i] = (Value - iEltMin)|HByte; // counter (left)
        
        // cout << "left counter:[" << (m_CArray[i]&XByte) << "]" << endl;
        // cout << "right counter:[" << (m_CArray[i+2]&XByte) << "]" << endl;
        
        m_CArray[i+3] = m_CArray[i+1] + Value - iEltMin + 1;
        
        if ((m_CArray[i]&XByte) == 1) {
            m_CArray[i] = m_CArray[i+1];
            m_CArray[i+1] = m_CArray[i+2];
            m_CArray[i+2] = m_CArray[i+3];
            iCollapseCounter++;
        }
        
        if ((m_CArray[i+2-iCollapseCounter]&XByte) == 1) {
            m_CArray[i+2-iCollapseCounter] = m_CArray[i+3-iCollapseCounter];
            iCollapseCounter++;
        }
        
        if (iCollapseCounter) {
            /* 09.09.1999: fix, interation m_CArrayMemoryFill-iCollapseCounter */
            for (register int j=i+2;j<m_CArrayMemoryFill-iCollapseCounter;j++)
                m_CArray[j] = m_CArray[j+iCollapseCounter];
            m_CArrayMemoryFill -= iCollapseCounter;
        }
        
        m_CArrayRealSize--;
    }
}

int CIntVector::FindElt(int Value) const {
    int iEltCount;
    int iEltMin;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) {
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = m_CArray[i+1];
            if ((iEltMin <= Value)&&(iEltMin+iEltCount-1 >= Value)) {
                // found in group
                return (i+(Value-iEltMin));
            }
            if (iEltMin+iEltCount-1>Value) return -1;
            i++;
        } else {
            if (m_CArray[i] == Value) {
                // remove single element
                return i;
            } else if (m_CArray[i] > Value) return -1;
        }
    }
    return -1;
}

bool CIntVector::DeleteElt(int Value) {
    int iEltCount;
    int iEltMin;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) {
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = m_CArray[i+1];
            if ((iEltMin <= Value)&&(iEltMin+iEltCount-1 >= Value)) {
                // found in group
                RemoveFromGroup(i, Value, iEltMin, iEltCount);
                return true;
            }
            if (iEltMin+iEltCount-1>Value) return false;
            i++;
        } else {
            if (m_CArray[i] == Value) {
                // remove single element
                RemoveSingle(i);
                return true;
            } else if (m_CArray[i] > Value) {
                return false;
            }
        }
    }
    return false;
}

void CIntVector::RemoveSingle(int i) {
    // collapse array on element i
    for (register int j=i;j<(m_CArrayMemoryFill-1);j++)
        m_CArray[j] = m_CArray[j+1];
    // we never need to merge, since we removed an intermediate element
    m_CArrayMemoryFill--;
    m_CArrayRealSize--;
}

bool CIntVector::DeleteEltAt(int Index) {
    int iCounter = 0, iEltCount, iEltMin;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) {
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = (m_CArray[i+1]);
            if ((int) (iCounter + iEltCount) > Index) {
                //cout << "removing: [" << iEltMin + (Index - iCounter) << "]" << endl;
                RemoveFromGroup(i, iEltMin + (Index - iCounter), m_CArray[i+1], iEltCount);
                return true;
            } else {
                iCounter+=iEltCount;
                i++;
            }
        } else if (iCounter == Index) {
            RemoveSingle(i);
            return true;
        }
        else iCounter++;
    }
    return false;
}

bool CIntVector::Contains(int Value) const {
    int iEltCount;
    int iEltMin;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) {
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = m_CArray[i+1];
            if ((iEltMin <= Value)&&(iEltMin+iEltCount-1 >= Value)) return 1;
            if (iEltMin+iEltCount-1>Value) return false;
            i++;
        } else {
            if (m_CArray[i] == Value) return true;
            else if (m_CArray[i] > Value) return false;
        }
    }
    return false;
}

void CIntVector::CCVResize(int DesiredSize) {
    if (DesiredSize > m_CArrayMemorySize) {
        int NewSize = DesiredSize;
        if (NewSize < MBYTE) {
            NewSize = (m_CArrayMemorySize?(m_CArrayMemorySize << 1):DesiredSize);
            while ( NewSize < DesiredSize)
                NewSize <<= 1;
        }
        int * NewArray = Allocate(NewSize);
        if (m_CArray) {
            memcpy(NewArray, m_CArray, sizeof(int) * m_CArrayMemoryFill);
            Free(m_CArray);
        }
        m_CArray = NewArray;
        m_CArrayMemorySize = NewSize;
    }    
}

bool CIntVector::InsertElt(int Value) {
    // cout << "Inserting " << Value << " into " << (* this) << endl;
    int iEltMin;
    int iEltCount;
    int MostLikelyPosition = 0;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) { // high bit at 1
            // if (Value == PAGE_REQ) { cout << "(1)" << endl; base_sleep(1); }
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = m_CArray[i+1];
            // element in the existing interval ?
            // cout << "min: " << iEltMin << " count: " << iEltCount << endl;
            if ((iEltMin <= Value)&&(iEltMin+iEltCount-1 >= Value)) {                
                return false;
            }
            // element bounding the interval (left) ?
            if ((iEltMin - 1) == Value) {
                // if (Value == PAGE_REQ) { cout << "(2)" << endl; base_sleep(1); }
                m_CArray[i] = (iEltCount+1) | (HByte);
                m_CArray[i+1] = Value;
                m_CArrayRealSize++;
                return true;
            } else if ((iEltMin + iEltCount) == Value) { // element bounding the interval (right) ?
                // if (Value == PAGE_REQ) { cout << "(3)" << endl; base_sleep(1); }
                m_CArray[i] = ((iEltCount+1) | (HByte));
                // cout << "new count: " << (m_CArray[i]&XByte) << endl;
                // check if we need to merge
                if (CCVPartOf(i+2, Value+1)) {
                    // if (Value == PAGE_REQ) { cout << "(4)" << endl; base_sleep(1); }
                    // merge the two groups/elements
                    if (m_CArray[i+2]&HByte) {
                        m_CArray[i] += (m_CArray[i+2]&XByte);
                        for (register int j=i+2;j<(m_CArrayMemoryFill-2);j++)
                            m_CArray[j] = m_CArray[j+2];
                        m_CArrayMemoryFill-=2;
                    } else {
                        // if (Value == PAGE_REQ) { cout << "(5)" << endl; base_sleep(1); }
                        // merging element is simple: increment the counter again
                        m_CArray[i]++;
                        for (int j=i+2;j<(m_CArrayMemoryFill-1);j++)
                            m_CArray[j] = m_CArray[j+1];
                        m_CArrayMemoryFill--;
                    }
                    CCVResize(m_CArrayMemoryFill);
                }
                m_CArrayRealSize++;
                return true;
            }
            i++;
            if (iEltMin + iEltCount < Value) MostLikelyPosition = i+1;
        } else {
            // if (Value == PAGE_REQ) { cout << "(6)" << endl; base_sleep(1); }
            // individual element
            if (m_CArray[i] == Value) {
                return false;
            } else if (m_CArray[i] - 1 == Value) {
                // if (Value == PAGE_REQ) { cout << "(7)" << endl; base_sleep(1); }
                CCVResize(m_CArrayMemoryFill+1);
                if (m_CArrayMemoryFill) {
                    for (register int j=m_CArrayMemoryFill-1;j>i;j--)
                        m_CArray[j+1] = m_CArray[j];
                }
                m_CArrayMemoryFill++;
                m_CArray[i] = 2 | (HByte);
                m_CArray[i+1] = Value;
                m_CArrayRealSize++;
                return true;
            } else if (m_CArray[i] + 1 == Value) {
                // if (Value == PAGE_REQ) { cout << "(8)" << endl; base_sleep(1); }
                CCVResize(m_CArrayMemoryFill+1);
                if (m_CArrayMemoryFill) {
                    for (register int j=m_CArrayMemoryFill-1;j>=i;j--)
                        m_CArray[j+1] = m_CArray[j];
                }
                m_CArrayMemoryFill++;
                m_CArray[i] = 2 | (HByte);
                // see if we need to merge
                if (CCVPartOf(i+2, Value+1)) {
                    // if (Value == PAGE_REQ) { cout << "(9)" << endl; base_sleep(1); }
                    // merge the two groups/elements
                    if (m_CArray[i+2]&HByte) {
                        // if (Value == PAGE_REQ) { cout << "(10)" << endl; base_sleep(1); }
                        m_CArray[i] += (m_CArray[i+2]&XByte);
                        for (register int j=i+2;j<(m_CArrayMemoryFill-2);j++)
                            m_CArray[j] = m_CArray[j+2];
                        m_CArrayMemoryFill-=2;
                    } else {
                        // if (Value == PAGE_REQ) { cout << "(11)" << endl; base_sleep(1); }
                        // merging elements: increment the counter again
                        m_CArray[i]++;
                        for (register int j=i+2;j<(m_CArrayMemoryFill-1);j++)
                            m_CArray[j] = m_CArray[j+1];
                        m_CArrayMemoryFill--;
                    }
                    CCVResize(m_CArrayMemoryFill);
                }
                m_CArrayRealSize++;
                return true;
            }
            if ((i < m_CArrayMemoryFill) && (m_CArray[i] < Value)) MostLikelyPosition = i+1;
        }
    }
    
    // nothing appropriate found
    // insert at MostlyLikelyPosition
    CCVResize(m_CArrayMemoryFill + 1);
    for (register int j=(m_CArrayMemoryFill-1);j>=MostLikelyPosition;j--)
        m_CArray[j+1] = m_CArray[j];
    m_CArray[MostLikelyPosition] = Value;
    m_CArrayMemoryFill++;
    m_CArrayRealSize++;
    return true;
}

void CIntVector::AppendVector(const CIntVector& Vector) {
    if ((&Vector == this) || (Vector.m_CArrayMemoryFill == 0))
        return;
    if (m_CArrayMemoryFill == 0) {
        operator=(Vector);
    } else {
        CCVResize(m_CArrayMemoryFill + Vector.m_CArrayMemoryFill);
        for (int i=0;i<Vector.m_CArrayMemoryFill;i++) {
            if (Vector.m_CArray[i]&HByte) {
                AddInterval(Vector.m_CArray[i+1], Vector.m_CArray[i+1] + (Vector.m_CArray[i]&XByte) - 1);
                i++;
            } else InsertElt(Vector.m_CArray[i]);
        }
    }
}

void CIntVector::AddInterval(int First, int Last) {
    //cout << "INTERVAL:[" << First << "-" << Last << "]" << endl;
    int l_Append = 1;
    if (m_CArrayRealSize >= 1) {
        int l_Last = m_CArray[m_CArrayMemoryFill-1];
        if ((m_CArrayMemoryFill >= 2)&&(m_CArray[m_CArrayMemoryFill-2]&HByte)) {
            l_Last += (m_CArray[m_CArrayMemoryFill-2]&XByte);
        }
        if (l_Last >= First) l_Append = 0;
    }
    if (l_Append) {
        CCVResize(m_CArrayMemoryFill + 2);
        m_CArray[m_CArrayMemoryFill] = (Last - First + 1) | (HByte);
        m_CArray[m_CArrayMemoryFill+1] = First;
        m_CArrayMemoryFill+=2;
        m_CArrayRealSize += (Last - First + 1);
    } else {
        for (register int i=First;i<=Last;i++) {
            InsertElt(i);
        }
    }
}

int * CIntVector::GetVector(void) const {
    // cout << "getting real vector from " << m_CArrayRealSize << "/" << m_CArrayMemoryFill << endl;
    if (m_CArrayRealSize) {
        int iEltCount;
        int * Result = new int[m_CArrayRealSize];
        int iCounter = 0;
        for (register int i=0;i<m_CArrayMemoryFill;i++) {
            if (m_CArray[i]&HByte) {
                iEltCount = (m_CArray[i]&XByte);
                for (int j=0;j<iEltCount;j++)
                    Result[iCounter++] = m_CArray[i+1] + j;
                i++;
            } else Result[iCounter++] = m_CArray[i];
        }
        assert(iCounter == m_CArrayRealSize);
        return Result;
    } else return NULL;
}

int CIntVector::GetEltAt(int Index) const {
    int iCounter = 0, iEltCount;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) {
            iEltCount = (m_CArray[i]&XByte);
            if (iCounter + iEltCount > Index) {
                return (m_CArray[i+1] + Index - iCounter);
            } else {
                iCounter+=iEltCount;
                i++;
            }
        } else if (iCounter == Index) return m_CArray[i];
        else iCounter++;
    }
    return 0;
}

istream& CIntVector::operator>>(istream& Stream) {
    assert(0);
    return Stream;
}

ostream& CIntVector::operator<<(ostream& Stream) const {
    int PrevValue = -1;
    int iEltCount, iEltMin;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) { // high bit at 1
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = m_CArray[i+1];
            if (PrevValue != -1) Stream << ' ';
            Stream << iEltMin << '-' << iEltMin+iEltCount-1;
            PrevValue = iEltMin+iEltCount-1;
            i++;
        } else {
            if (PrevValue != -1) Stream << ' ';
            PrevValue = m_CArray[i];
            Stream << m_CArray[i];
        }
    }
    return Stream;
}

void CIntVector::_AppendElt(int Value) {
    CCVResize(m_CArrayMemoryFill+1);
    m_CArray[m_CArrayMemoryFill] = Value;
    m_CArrayMemoryFill++;
    m_CArrayRealSize++;
}

void CIntVector::_AppendInt(int First, int Last) {
    CCVResize(m_CArrayMemoryFill+2);
    m_CArray[m_CArrayMemoryFill] = (Last - First + 1) | (HByte);
    m_CArray[m_CArrayMemoryFill+1] = First;
    m_CArrayMemoryFill+=2;
    m_CArrayRealSize+=(Last - First + 1);
}

void CIntVector::Write(FILE * Descr) const {
    int PrevValue = -1;
    int iEltCount, iEltMin;
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) { // high bit at 1
            iEltCount = (m_CArray[i]&XByte);
            iEltMin = m_CArray[i+1];
            if (PrevValue != -1) fprintf(Descr, " ");
            fprintf(Descr, "%d-%d", iEltMin, iEltMin+iEltCount-1);
            PrevValue = iEltMin+iEltCount-1;
            i++;
        } else {
            if (PrevValue != -1) fprintf(Descr, " ");
            PrevValue = m_CArray[i];
            fprintf(Descr, "%d", m_CArray[i]);
        }
    }
}

CVector<int>& CIntVector::AppendTo(CVector<int>& Target) const {
    int iEltCount;
    Target.SetDim(Target.GetSize() + GetSize());	
    for (register int i=0;i<m_CArrayMemoryFill;i++) {
        if (m_CArray[i]&HByte) {
            iEltCount = m_CArray[i]&XByte;
            for (int j=0;j<iEltCount;j++) Target += m_CArray[i+1] + j;
            i++;
        } else Target += m_CArray[i];
    }
    return Target;
}
