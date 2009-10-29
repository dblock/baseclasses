/*

  Memory Mapped File
  ==================================================
  part of the Alkaline Search Engine
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include <File/LocalFile.hpp>
#include <Object/Tracer.hpp>
#include "MMapFile.hpp"

CMMapFile::CMMapFile(const CString& Filename) :
    m_Mem(NULL)
    , m_Offset(0)
    , m_Size(0)
#ifdef _UNIX
    , m_hFileHandle(-1)
#endif
#ifdef _WIN32
    , m_hFileMappingObject(NULL)
    , m_hFileHandle(NULL)
#endif
{
    m_Filename = Filename;
}

CMMapFile::~CMMapFile(void) {
    MMUnmap();
}

void CMMapFile::MMUnmap(void) {

#ifdef _UNIX
  
    Trace(tagSystem, levInfo, ("CMMapFile::MMUnmap - size: %ld byte(s), memory: %#x, file handle: %#x.", m_Size, m_Mem, m_hFileHandle));

    if (m_Mem)
        munmap((caddr_t) m_Mem, m_Size);
    if (m_hFileHandle > 0)
        base_close(m_hFileHandle);
#endif
#ifdef _WIN32
    
    Trace(tagSystem, levInfo, ("CMMapFile::MMUnmap - size: %ld byte(s), memory: %#x, file handle: %#x, mapping: %#x.", m_Size, m_Mem, m_hFileHandle, m_hFileMappingObject));
    
    if (m_Mem)
        UnmapViewOfFile(m_Mem);
    if (m_hFileMappingObject)
        CloseHandle(m_hFileMappingObject);
    if (m_hFileHandle) 
        CloseHandle(m_hFileHandle);
#endif
    m_Size = 0;
    m_Offset = 0;
    m_Mem = 0;
}

bool CMMapFile::MMap(int MapMode, long Resize) {

  int FileMode = 0;

  // get the size
  if (m_Size)
    MMUnmap();

  m_Size = (Resize == -1) ? CLocalFile::GetFileSize(m_Filename) : Resize;

  Trace(tagSystem, levInfo, ("CMMapFile::MMap - size %ld byte(s).", m_Size));

  m_Offset = 0;
  
  // open the file
#ifdef _UNIX
    
  // a file beeing resized needs to be created/truncated
  if (Resize >= 0)
    FileMode |= (O_CREAT | O_TRUNC);
  
  // it looks like on some Linuxes mmap is implemented like on NT and
  // requies a write access to the file because it can't protect
  // a memory block
  m_hFileHandle = base_open((const char *) m_Filename.GetBuffer(), MapMode | FileMode, S_IWUSR | S_IRUSR);
  if (m_hFileHandle == -1) {        
    Trace(tagSystem, levError, ("CMMapFile::MMap - failed to open %s.", m_Filename.GetBuffer()));
    return false;
  }
#endif
#ifdef _WIN32
  
  FileMode = (Resize >= 0) ? CREATE_ALWAYS : OPEN_EXISTING;    

  m_hFileHandle = CreateFile(
    m_Filename.GetBuffer(), 
	MapMode,
	0, 
	NULL, 
	FileMode, 
    FILE_FLAG_RANDOM_ACCESS,
    0);
  
  if (!m_hFileHandle) {
    Trace(tagSystem, levError, ("CMMapFile::MMap - CreateFile failed."));
    return false;
  }
#endif
  
  // seek within the file if asked    
  if (Resize > 0) {

	#ifdef _WIN32
	  if (SetFilePointer(m_hFileHandle, Resize, NULL, FILE_BEGIN) != Resize) {
		 Trace(tagSystem, levError, ("CMMapFile::MMap - seek (1) failed."));
		 return false;
	  }
	  if (! SetEndOfFile(m_hFileHandle)) {
		 Trace(tagSystem, levError, ("CMMapFile::MMap - SetEndOfFile failed."));
		 return false;
	  }
	#endif
	#ifdef _UNIX
    // seek to the end of file
	  if (base_seek((int) m_hFileHandle, Resize - 1, SEEK_SET) != Resize - 1) {
	    Trace(tagSystem, levError, ("CMMapFile::MMap - seek (1) failed."));
		return false;
	  }    
	  // write a byte
	  if (base_write((int) m_hFileHandle, "\0", 1) != 1) {
		  Trace(tagSystem, levError, ("CMMapFile::MMap - write failed."));
		  return false;
	  }
	  if (base_seek((int) m_hFileHandle, Resize - 1, SEEK_SET) != Resize - 1) {
		  Trace(tagSystem, levError, ("CMMapFile::MMap - seek (2) failed."));
		  return false;
	  }
	#endif
  }
  
  // map the region
#ifdef _UNIX
  m_Mem = (char *) mmap(
        0, 
        m_Size, 
        ((MapMode & O_RDWR) || (MapMode & O_WRONLY)) ? (PROT_READ | PROT_WRITE | PROT_EXEC) : PROT_READ,
        MAP_SHARED, 
        m_hFileHandle, 
        0);
  
  if (m_Mem == (void *) -1) {
    Trace(tagSystem, levError, ("CMMapFile::MMap - mmap failed."));
    return false;    
  }
#endif
  
#ifdef _WIN32
  m_hFileMappingObject = CreateFileMapping(
            m_hFileHandle, 
            NULL, 
            (MapMode & GENERIC_WRITE) ? PAGE_READWRITE : PAGE_READONLY, 
            0, 
            m_Size, 
            NULL);

  if (! m_hFileMappingObject) {
    Trace(tagSystem, levError, ("CMMapFile::MMap - CreateFileMapping with %ld bytes failed.", m_Size));
    return false;
  }
  
  m_Mem = (void *) MapViewOfFile(
	m_hFileMappingObject, 
        (MapMode & GENERIC_WRITE) ? FILE_MAP_WRITE : FILE_MAP_READ,
        0, 
        0,
        m_Size);

  if (!m_Mem) {
    Trace(tagSystem, levError, ("CMMapFile::MMap - MapViewOfFile with %ld bytes failed.", m_Size));    
    return false;
  }
#endif
  
  Trace(tagSystem, levInfo, ("CMMapFile::MMap - MMap suceeded for %s, size is %ld bytes.", m_Filename.GetBuffer(), m_Size));
  return true;
}

 
long CMMapFile::ReadLine(CString * pLine) const {
  pLine->Empty();
  
  if (!m_Mem || (m_Offset >= m_Size))
    return -1;
  
  char ch;
  do {
    ch = ((char *) m_Mem)[m_Offset];
    m_Offset++;
    if ((ch == 0) || (ch == 10)) {
      return pLine->GetLength();
    } else {
      if (ch != 13)
        pLine->Append(ch);
    }
  } while (m_Offset < m_Size);
  return pLine->GetLength();
}

long CMMapFile::Read(CString * pLine, unsigned int Bytes) const {
  pLine->Empty();
  if (!m_Mem)
    return -1;  
  if ((long) (m_Offset + Bytes) >= m_Size)
    Bytes = m_Size - m_Offset;  
  
  pLine->SetLength(Bytes);
  pLine->CopyBuffer((char *) m_Mem + m_Offset, Bytes);
  m_Offset += Bytes;
  
  return pLine->GetLength();  
}

long CMMapFile::Read(void * pBuffer, unsigned int Bytes) const {
    if (!m_Mem)
        return -1;
    
    if ((long) (m_Offset + Bytes) >= m_Size)
        Bytes = m_Size - m_Offset;  
    
    memcpy(pBuffer, (char *) m_Mem + m_Offset, Bytes);
    m_Offset += Bytes;
    
    return Bytes;
}

bool CMMapFile::Write(void * pBuffer, unsigned int Bytes) {
  if (! m_Mem || (long) Bytes + m_Offset > m_Size)
    return false;
  memcpy((char *) m_Mem + m_Offset, pBuffer, Bytes);
  m_Offset += Bytes;
  return true;
}

long CMMapFile::Seek(long Offset, int Whence) const {
  if (!m_Mem)
    return false;
  
  long CurrentOffset = m_Offset;
  switch(Whence) {
  case SEEK_SET:
    CurrentOffset = Offset;
    break;
  case SEEK_CUR:
    CurrentOffset += Offset;
    break;
  case SEEK_END:
    CurrentOffset = m_Size + Offset - 1;
    break;
  default:
    return false;
  }
  
  if ((CurrentOffset >= 0) && (CurrentOffset < m_Size)) {
    m_Offset = CurrentOffset;
    return true;
  }
  
  return false;
}
