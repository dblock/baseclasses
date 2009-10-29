/*

  Memory Mapped File
  ==================================================
  part of the Alkaline Search Engine
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef MMAP_FILE_HPP
#define MMAP_FILE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

#ifdef _UNIX
#define MMAP_OPENMODE O_RDWR
#define MMAP_READOPENMODE O_RDONLY
#endif

#ifdef _WIN32
#define MMAP_OPENMODE GENERIC_READ | GENERIC_WRITE
#define MMAP_READOPENMODE GENERIC_READ
#endif                  
    
class CMMapFile : public CObject {
    property(CString, Filename);
    void * m_Mem;
    mutable long m_Offset;
    readonly_property(long, Size);    
#ifdef _UNIX
    int m_hFileHandle;
#endif
#ifdef _WIN32
    HANDLE m_hFileMappingObject;
    HANDLE m_hFileHandle;
#endif
public:
    CMMapFile(const CString& Filename);
    virtual ~CMMapFile(void);
    bool MMap(int MapMode = MMAP_OPENMODE, long Resize = -1);
    void MMUnmap(void);
    inline long GetOffset(void) const { return m_Offset; }
    inline const void * GetMem(void) const { return m_Mem; }
    long ReadLine(CString * pLine) const;
    long Read(CString * pLine, unsigned int Bytes) const;
    long Read(void * pBuffer, unsigned int Bytes) const;
    bool Write(void * pBuffer, unsigned int Bytes);
    long Seek(long Offset = 0, int Whence = SEEK_SET) const;
    inline long Rewind(void) const { return Seek(0, SEEK_SET); }
};
 
#endif
