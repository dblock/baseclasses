/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com
  
    Revision history:
    
    26.05.2000: Read() will not allocate the buffer twice, does it directly to the string
    21.06.2000: temp unix file names fixes
      
*/

#include <baseclasses.hpp>

#include "LocalFile.hpp"
#include "Progress.hpp"

#define DEFAULT_LINE_SIZE 64

CLocalFile::CLocalFile(const CString& Filename) {
    SetFilename(Filename);
}

CLocalFile::CLocalFile(void) {
    m_FileHandle = -1;
    m_HttpError = 200;
}

void CLocalFile::SetFilename(const CString& Filename) {    
    Trace(tagSystem, levInfo, ("CLocalFile::SetFilename [%s]", Filename.GetBuffer()));
    m_LocalPath = Filename;
    m_FileHandle = -1;
    m_HttpError = 200;
}

CLocalFile::~CLocalFile(void) {
    Close();
}

bool CLocalFile::Open(int Flags, mode_t Mode) const {
    if (m_FileHandle != -1) Close();
    m_FileHandle = base_open((const char *) GetFullPath().GetBuffer(), Flags, Mode);
    Trace(tagSystem, levInfo, ("CLocalFile::Open [%s][%d]", GetFullPath().GetBuffer(), m_FileHandle));
    if (m_FileHandle == -1) {
        Error();
        return false;
    } else {
        m_HttpError = 200;
        m_StringError.Empty();
        return true;
    }
}

bool CLocalFile::Close(void) const {    
    bool Result = false;
    Trace(tagSystem, levInfo, ("CLocalFile::Close [%d]", m_FileHandle));
    if (m_FileHandle != -1) {
        Result = (base_close(m_FileHandle) == 0);
        m_FileHandle = -1;
    }
    return Result;
}

off_t CLocalFile::Seek(off_t Offset, int Whence) const {
    if (Verify()) {
      if (m_FileHandle == -1)
        return -1;
      else return base_seek(m_FileHandle, Offset, Whence);
    } else return -1;
}

off_t CLocalFile::GetSize(bool Preserve) const {
  if (Verify()) {
    off_t CurrentOffset = 0; 
    if (Preserve) 
      CurrentOffset = GetOffset();
    off_t Result = Forward();
    if (Preserve) 
      Seek(CurrentOffset);

    Trace(tagSystem, levInfo, ("CLocalFile::GetSize [%s][%ld bytes]", GetFullPath().GetBuffer(), Result));

    return Result;
  } else return -1;
}

int CLocalFile::Read(CString * pResult) const {
  off_t lFileSize = GetSize(false);
  if (lFileSize == -1)
    pResult->Empty();
  else return Read(pResult, lFileSize, 0);
  return 0;
}

int CLocalFile::Read(CString * pResult, const int Bytes, const off_t Offset) const {
    pResult->Empty();
    if (Verify()) {
        if (Bytes > 0) {
            if (Offset != -1)
              if (Seek(Offset) == -1)
                return 0;
            pResult->SetLength(Bytes);
            ssize_t ReadBytes = base_read(m_FileHandle, (void *) pResult->GetBuffer(), Bytes);
            if (ReadBytes > 0)
              pResult->SetLength(ReadBytes);
            else pResult->Empty();
        }

        Trace(tagSystem, levInfo, ("CLocalFile::Read [%d bytes]", pResult->GetLength()));

        return pResult->GetLength();
    } else return 0;
}

int CLocalFile::ReadLine(CString * pResult, const off_t Offset) const {
   pResult->Empty();
   if (Verify()) {
        if (Offset != -1) 
          if (Seek(Offset) == -1) 
            return 0;
        char Buffer[DEFAULT_LINE_SIZE + 1];
        char c;
        int CurrentBufferPosition = 0;
        while (base_read(m_FileHandle, (void *) &c, 1) == 1) {
            if (c == 10) break;
            else if (c != 13) {
                Buffer[CurrentBufferPosition] = c;
                CurrentBufferPosition++;
            }
            if (CurrentBufferPosition == DEFAULT_LINE_SIZE) {
                Buffer[CurrentBufferPosition] = 0;
                pResult->Append(Buffer, CurrentBufferPosition);
                CurrentBufferPosition = 0;
            }
        }
        Buffer[CurrentBufferPosition] = 0;
        if (CurrentBufferPosition) 
          pResult->Append(Buffer, CurrentBufferPosition);

        Trace(tagSystem, levInfo, ("CLocalFile::ReadLine [%d bytes]", pResult->GetLength()));

        return pResult->GetLength();
    } else return 0;
}

unsigned char CLocalFile::ReadChar(const off_t Offset) const {
    if (Verify()) {
      if (Offset != -1) 
        if (Seek(Offset) == -1)
          return 0;
      char Result;
      if (base_read(m_FileHandle, (void *)&Result, 1) == 1) return Result;
    }
    return 0;
}

bool CLocalFile::Write(const CString& Buffer, const off_t Offset) const {
    if (Buffer.GetLength() && Verify()) {
      if (Offset != -1) 
        if (Seek(Offset) == -1)
          return false;
      return (base_write(m_FileHandle, Buffer.GetBuffer(), Buffer.GetLength()) == (int) Buffer.GetLength());
    } else return false;
}

bool CLocalFile::Write(void * pBuffer, const int Bytes) const {
	if (Verify()) {
		return (base_write(m_FileHandle, pBuffer, Bytes) == Bytes);
	} else return false;
}

bool CLocalFile::WriteLine(const CString& Buffer, const off_t Offset) const {
    if (Buffer.EndsWith("\n")) return Write(Buffer, Offset);
    else if (Buffer.GetLength() && Verify()) {
      if (Offset != -1) 
        if (Seek(Offset) == -1)
          return false;
      return (base_write(m_FileHandle, (Buffer + "\n").GetBuffer(), Buffer.GetLength() + base_strlen("\n")) == (int) ((int) Buffer.GetLength() + sizeof("\n") - 1));
    } else return false;
}

void CLocalFile::Error(void) const {
    m_HttpError = 500;
    switch(base_errno) {
#ifdef _WIN32
    case EACCES:
        m_HttpError = 404;
        m_StringError = "Tried to open read-only file for writing, or file’s sharing mode does not allow specified operations, or given path is directory";
        break;
    case EEXIST:
        m_HttpError = 403;
        m_StringError = "_O_CREAT and _O_EXCL flags specified, but filename already exists";
        break;
    case EINVAL:
        m_HttpError = 403;
        m_StringError = "Invalid oflag or pmode argument";
        break;
    case EMFILE:
        m_HttpError = 403;
        m_StringError = "No more file handles available (too many open files)";
        break;
    case ENOENT:
        m_HttpError = 404;
        m_StringError = "File or path not found";
        break;
#endif
#ifdef _UNIX
    case EACCES:
        m_HttpError = 403;
        m_StringError = "Search permission is denied on a component of " \
            "the  path  prefix, or the file exists and the " \
            "permissions specified by oflag are denied, or " \
            "the  file does not exist and write permission " \
            "is denied for the  parent  directory  of  the " \
            "file  to  be created, or O_TRUNC is specified " \
            "and write permission is denied.";
        break;
    case EDQUOT:
        m_HttpError = 404;
        m_StringError = "The file does not exist,  O_CREAT  is  specified, " \
            "and either the directory where the new " \
            "file entry is being placed cannot be extended " \
            "because  the  user's  quota of disk blocks on " \
            "that file system has been exhausted,  or  the " \
            "user's  quota  of  inodes  on the file system " \
            "where the file  is  being  created  has  been " \
            "exhausted.";
        break;
    case EEXIST:
        m_StringError = "O_CREAT and O_EXCL are  set,  and  the  name file exists.";
        break;
    case EINTR:
        m_HttpError = 404;
        m_StringError = "A signal was caught during open().";
        break;
    case EFAULT:
        m_HttpError = 404;
        m_StringError = "Path points to an illegal address.";
        break;
    case EIO:
        m_HttpError = 404;
        m_StringError = "The path argument names a STREAMS file and  a hangup or error occurred during the open().";
        break;
    case EISDIR:
        m_HttpError = 404;
        m_StringError = "The named  file  is  a  directory  and  oflag includes O_WRONLY or O_RDWR.";
        break;
    case ELOOP:
        m_HttpError = 404;
        m_StringError = "Too many symbolic links were  encountered  in resolving path.";
        break;
    case EMFILE:
        m_HttpError = 503;
        m_StringError = "OPEN_MAX file descriptors are currently open in the calling process.";
        break;
#ifdef EMULTIHOP
    case EMULTIHOP:
        m_HttpError = 503;
        m_StringError = "Components of path require hopping to  multiple  remote machines and the file system does not allow it.";
        break;
#endif
    case ENAMETOOLONG:
        m_HttpError = 404;
        m_StringError = "The  length  of  the  path  argument  exceeds PATH_MAX  or  a  pathname component is longer than NAME_MAX.";
        break;
    case ENFILE:
        m_HttpError = 503;
        m_StringError = "The maximum  allowable  number  of  files  is currently open in the system.";
        break;
    case ENOENT:
        m_HttpError = 404;
        m_StringError = "O_CREAT is not set and the  named  file  does" \
            " not  exist;  or O_CREAT is set and either the" \
            " path prefix does not exist or the path  argument points to an empty string.";
        break;
#ifdef ENOLINK
    case ENOLINK:
        m_HttpError = 404;
        m_StringError = "path points to a remote machine, and the link " \
            "to that machine is no longer active.";
        break;
#endif
#ifdef ENOSR
    case ENOSR:
        m_HttpError = 503;
        m_StringError = "The path argument names a STREAMS-based  file and  the  system  is  unable  to  allocate  a STREAM.";
        break;
#endif
    case ENOSPC:
        m_HttpError = 404;
        m_StringError = "The directory or file system that would  contain " \
            "the  new  file  cannot be expanded, the " \
            "file does not exist, and  O_CREAT  is  specified.";
        break;
    case ENOTDIR:
        m_HttpError = 404;
        m_StringError = "A component of  the  path  prefix  is  not a directory.";
        break;
        /*
        case ENXIO:
        m_StringError = "O_NONBLOCK is set, the named file is a  FIFO, " \
        "O_WRONLY  is  set and no process has the file " \
        "open for reading.";
        break;
        */
    case ENXIO:
        m_HttpError = 404;
        m_StringError = "The named file  is  a  character  special  or block " \
            "special file, and the device associated " \
            "with this special file does not exist.";
        break;
    case EOPNOTSUPP:
        m_StringError = "An attempt was  made  to  open  a  path  that corresponds to a AF_UNIX socket.";
        break;
#ifdef EOVERFLOW
    case EOVERFLOW:
        m_StringError = "The named file is a regular file  and  either "\
            "O_LARGEFILE  is  not  set and the size of the " \
            "file cannot be represented  correctly  in  an " \
            "object  of  type  off_t or O_LARGEFILE is set " \
            "and  the  size  of   the   file   cannot   be " \
            "represented  correctly  in  an object of type " \
            "off64_t.";
        break;
#endif
    case EROFS:
        m_StringError = "The named file resides on  a  read-only  file " \
            "system  and  either O_WRONLY, O_RDWR, O_CREAT " \
            "(if file does not exist), or O_TRUNC  is  set " \
            "in the oflag argument.";
        break;
    case EAGAIN:
        m_StringError = "The path argument names the slave side  of  a "
            "pseudo-terminal device that is locked.";
        break;
    case EINVAL:
        m_StringError = "The value of the oflag argument is not valid.";
        break;
        /*
        case ENAMETOOLONG:
        m_HttpError = 404;
        m_StringError = "Pathname resolution of a symbolic  link  produced  an  intermediate  result  whose length exceeds PATH_MAX.";
        break;
        */
    case ENOMEM:
        m_StringError = "The path argument names a  STREAMS  file  and the system is unable to allocate resources.";
        break;
    case ETXTBSY:
        m_StringError = "The file is a pure  procedure  (shared  text) file  that  is  being  executed  and oflag is O_WRONLY or O_RDWR.";
        break;
#endif	
    default:
        m_StringError.Empty();
        break;
    }
    
}

bool CLocalFile::FileExists(const CString& Filename) {
    struct_stat Stat;
	bool fExists = (base_stat((const char *) Filename.GetBuffer(), &Stat) != -1);

	Trace(tagSystem, levInfo, ("CLocalFile::FileExists [%s][%s]", Filename.GetBuffer(), fExists ? "yes" : "no"));

    return fExists;
}

long CLocalFile::GetFileSize(const CString& Filename) {
    struct_stat Stat;
    if (base_stat((const char *) Filename.GetBuffer(), &Stat) != -1) {

        Trace(tagSystem, levInfo, ("CLocalFile::GetFileSize [%s][%d bytes]", Filename.GetBuffer(), Stat.st_size));

        return Stat.st_size;
    } else return -1;
}

bool CLocalFile::GetFileStat(const CString& Filename, struct_stat& Stat) {
  
  Trace(tagSystem, levInfo, ("CLocalFile::GetFileStat [%s]", Filename.GetBuffer()));

  return (base_stat((const char *) Filename.GetBuffer(), &Stat) != -1);
}

bool CLocalFile::Create(void) const {
    m_LocalPath.ForceDirectory();
#ifdef _WIN32
    return Open(O_CREAT | O_TRUNC | O_RDWR | O_BINARY);
#endif
#ifdef _UNIX
    return Open(O_CREAT | O_TRUNC | O_RDWR);
#endif	
}

int CLocalFile::ReadLines(CVector<CString> * pResult, bool Verbose) const {
    pResult->RemoveAll();
    if (GetSize() > 0) {
        CProgress Progress(20, Verbose);
        CString Contents;
        Read(&Contents);
        pResult->SetDim(Contents.GetCount(10));
        int Pos = 0;
        CString Line;
        while (Pos < (int) Contents.GetLength()) {
            Progress.Show(Pos, Contents.GetLength(), Verbose);
            Contents.GetLine(&Line, Pos);
            (* pResult) += Line;
        }
        Progress.Finish(Verbose);
    }	
    return pResult->GetSize();
}

#ifdef _UNIX
CString CLocalFile::GetTmpPrefix(void) {
  CString Filename;
  Filename = "/tmp/blib-";
  Filename += CString::LongToStr(base_getpid());
  Filename += '-';
  return Filename;  
}
#endif

CString CLocalFile::GetTmpName(void) {
    static CAtomic m_TmpFilenameAtomic;
    CString Filename;
    long NextTmpFilename = m_TmpFilenameAtomic.Inc();
#ifdef _UNIX        
    Filename = "/tmp/blib-";
    Filename += CString::LongToStr(base_getpid());
    Filename += '-';
    Filename += CString::LongToStr(NextTmpFilename);        
#endif
#ifdef _WIN32
    NextTmpFilename += base_getpid(); 
    char * tmpFilepath[MAX_PATH];
    tmpFilepath[0] = 0;
    GetTempPath(MAX_PATH, (char *) tmpFilepath);
    char szFilename[MAX_PATH];
    if (base_strlen((char *) tmpFilepath)) {
        GetTempFileName((char *) tmpFilepath, "~BLIB", NextTmpFilename, szFilename);
        Filename = szFilename;
        Filename += '-';
        Filename += CString::LongToStr(base_getpid()); 
    } else Filename = tmpnam(NULL);	
#endif
    return Filename;
}

void CLocalFile::GetTmpNameNCS(char szName[MAXPATHLEN]) {
    static CAtomic m_TmpFilenameAtomic;
    long NextTmpFilename = m_TmpFilenameAtomic.Inc();
#ifdef _UNIX        
    sprintf(szName, "/tmp/blib-%d-%ld", base_getpid(), NextTmpFilename);
#endif
#ifdef _WIN32
    NextTmpFilename += base_getpid(); 
    char * tmpFilepath[MAX_PATH];
    tmpFilepath[0] = 0;
    GetTempPath(MAX_PATH, (char *) tmpFilepath);
    char szFilename[MAX_PATH];
    if (base_strlen((char *) tmpFilepath)) {
        GetTempFileName((char *) tmpFilepath, "~BLIB", NextTmpFilename, szFilename);
        sprintf(szName, "%s-%d", szFilename, base_getpid());        
    } else sprintf(szName, "%s", tmpnam(NULL));
#endif    
}

int CLocalFile::GetTmpDescriptor(CString * pFilename) {
    CString TmpFilename = GetTmpName();
    
    if (pFilename) 
        * pFilename = TmpFilename;
    
    if (!TmpFilename.GetLength())
        return -1;
    
#ifdef _UNIX
    return base_open((const char *) TmpFilename.GetBuffer(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
#endif
#ifdef _WIN32
    return base_open((const char *) TmpFilename.GetBuffer(), O_RDWR | _O_CREAT | O_TRUNC, _O_TEMPORARY);
#endif
    
}

bool CLocalFile::GetTmpNameSz(char szName[MAXPATHLEN]) {
  CString TmpName = GetTmpName();
  if (TmpName.GetLength() && (TmpName.GetLength() < MAXPATHLEN)) {
    memcpy(szName, TmpName.GetBuffer(), TmpName.GetLength() + 1);
    return true;
  } else szName[0] = 0;
  return false;
}

int CLocalFile::GetTmpDescriptorSz(char szName[MAXPATHLEN]) { 
  CString TmpName;
  int Descriptor = GetTmpDescriptor(&TmpName);
  if (TmpName.GetLength() && (TmpName.GetLength() < MAXPATHLEN)) {
    memcpy(szName, TmpName.GetBuffer(), TmpName.GetLength() + 1);
  } else szName[0] = 0;  
  return Descriptor;
}

#ifdef _WIN32
HANDLE CLocalFile::GetTmpHandle(CString * pFilename) {
    CString TmpFilename = GetTmpName();
    
    if (pFilename) 
        * pFilename = TmpFilename;
    
    if (!TmpFilename.GetLength())
        return 0;
    
    return CreateFile(TmpFilename.GetBuffer(), 
        GENERIC_ALL,
        0, NULL, CREATE_ALWAYS, 
        FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE,
        0);
}

HANDLE CLocalFile::GetTmpHandleSz(char szName[MAXPATHLEN]) {
    GetTmpNameNCS(szName);
    
    return CreateFile(szName, 
        GENERIC_ALL,
        0, NULL, CREATE_ALWAYS, 
        FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE,
        0);
}

#endif

bool CLocalFile::CreateAndResize(const CString& Filename, off_t Size) {
	// create a file and resize it accordingly	
	CLocalFile File(Filename);
	if (! File.Create())
		return false;
	if (! Size)
		return true;
	// seek to end of file
	if (File.Seek((off_t) Size - 1, SEEK_SET) != Size - 1)
		return false;
	// write a byte to resize the file
	if (! File.Write(& Size, 1))
		return false;
	File.Close();
	return true;
}

