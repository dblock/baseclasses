#ifndef RW_NAMED_TABLE_HPP
#define RW_NAMED_TABLE_HPP

#include <platform/include.hpp>
#include <Mutex/RWMutex.hpp>
#include <HashTable/HashTable.hpp>

#define RW_NAMEDTABLE_SIZE 48
 
class CRWNamedMutex {
    friend class CRWNamedTable;
private:
    long m_RefCount;
    CString m_Name;
    CRWMutex m_Mutex;
public:
    CRWNamedMutex(void);
    ~CRWNamedMutex(void);
};

typedef enum { mtxStartReading, mtxStopReading, mtxStartWriting, mtxStopWriting } CMutexOpType;

class CRWNamedTable : public CObject {
private:    
  CMutex m_TableMutex;
  CVector<CRWNamedMutex *> m_NamedList;
public:
  CRWNamedTable(void);
  ~CRWNamedTable(void);
  inline void StartReading(const CString& Name);
  inline void StartWriting(const CString& Name);
  inline void StopReading(const CString& Name);
  inline void StopWriting(const CString& Name);
  void PerformMutex(const CString& Name, CMutexOpType OpType);
};

inline void CRWNamedTable :: StartReading(const CString& Name) {
  PerformMutex(Name, mtxStartReading);
}
 
inline void CRWNamedTable :: StartWriting(const CString& Name) {
  PerformMutex(Name, mtxStartWriting);
}
 
inline void CRWNamedTable :: StopReading(const CString& Name) {
  PerformMutex(Name, mtxStopReading);
}

inline void CRWNamedTable :: StopWriting(const CString& Name) {
  PerformMutex(Name, mtxStopWriting);
}


#endif
