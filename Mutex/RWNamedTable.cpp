#include <baseclasses.hpp>
#include "RWNamedTable.hpp"

CRWNamedMutex :: CRWNamedMutex(void) :
  m_RefCount(0) {

}

CRWNamedMutex :: ~CRWNamedMutex(void) {

}


CRWNamedTable :: CRWNamedTable(void) {
  
}

CRWNamedTable :: ~CRWNamedTable(void) {
  
}

void CRWNamedTable :: PerformMutex(const CString& Name, CMutexOpType OpType) {

  CRWNamedMutex * pMutex = NULL;

  m_TableMutex.Lock();
  
  for (int i = 0; i < (int) m_NamedList.GetSize(); i++) {
      if (m_NamedList[i]->m_Name == Name) {
          pMutex = m_NamedList[i];
          pMutex->m_RefCount++;
          break;
      }
  }

  if (! pMutex) {
      pMutex = new CRWNamedMutex;
      pMutex->m_Name = Name;  
      pMutex->m_RefCount++;
      m_NamedList.Add(pMutex);
  }

  m_TableMutex.UnLock();

  switch(OpType) {
  case mtxStartReading:
      pMutex->m_Mutex.StartReading();      
      break;
  case mtxStopReading:      
      pMutex->m_Mutex.StopReading();            
      break;
  case mtxStartWriting:      
      pMutex->m_Mutex.StartWriting();      
      break;
  case mtxStopWriting:
      pMutex->m_Mutex.StopWriting();      
      break;
  }
  
  m_TableMutex.Lock();

  switch(OpType) {
  case mtxStartReading:
  case mtxStartWriting:      
      pMutex->m_RefCount++;
      break;
  case mtxStopReading:      
  case mtxStopWriting:
      pMutex->m_RefCount--;
      break;
  }

  pMutex->m_RefCount--;

  if (pMutex->m_RefCount == 0) {
      m_NamedList.Remove(pMutex);
      delete pMutex;
  }

  m_TableMutex.UnLock();

}

