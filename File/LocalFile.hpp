/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
  Revision history:
  
  28.08.1999: default mode for opening files for writing is binary under Windows NT  
  29.08.1999: added GetFileSize and GetFileStat
  02.08.1999: Create will force the directory first
  14.09.1999: added Delete() - unlink file

*/

#ifndef BASE_LOCAL_FILE_HPP
#define BASE_LOCAL_FILE_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <Mutex/Atomic.hpp>
#include "LocalPath.hpp"

class CLocalFile : public CObject {
	mutable_property(CString, StringError);
	mutable_property(int, HttpError);
	readonly_property(CLocalPath, LocalPath);
	mutable_private_property(int, FileHandle);
private:
	void Error(void) const;
public:
	/* name queries */
	inline CString GetPath(void) const { return m_LocalPath.GetPath(); }
	inline CString GetFilename(void) const { return m_LocalPath.GetFilename(); }
	inline CVector<CString> GetPathVector(void) const { return m_LocalPath.GetPathVector(); }
	// inline CString GetPathFull(void) const { return m_LocalPath.GetPathFull(); }
	inline CString GetFullPath(void) const { return m_LocalPath.GetFullPath(); }
	/* constructors */
	CLocalFile(const CString&);
    CLocalFile(void);
	virtual ~CLocalFile(void);
    void SetFilename(const CString&);
	/* open for read/write/append, close */
	bool Create(void) const;
	static bool CreateAndResize(const CString& Filename, off_t Size);
    inline bool Delete(void) const { Close(); return (base_unlink((const char *) GetFullPath().GetBuffer()) == 0); }
    inline static bool Delete(const CString& Filename) { return (base_unlink((const char *) Filename.GetBuffer()) == 0); }
#ifdef _WIN32	
	bool Open(int = O_RDWR | O_BINARY, mode_t = _S_IREAD | _S_IWRITE) const;
    inline bool OpenReadBinary(void) const { return Open(O_RDONLY | O_BINARY); }
#endif
#ifdef _UNIX	
	bool Open(int = O_RDWR, mode_t = S_IRUSR | S_IWUSR) const;
    inline bool OpenReadBinary(void) const { return Open(O_RDONLY); }
#endif
    inline bool IsOpened(void) const { return (m_FileHandle != -1) ? true : false; }
	inline bool Verify(void) const { if (m_FileHandle == -1) return Open(); else return true; }
	bool Close(void) const;
	/* seeking */
	off_t Seek(off_t = 0, int = SEEK_SET) const;
	inline off_t Rewind(void) const { return Seek(0, SEEK_SET); }
	inline off_t Forward(void) const { return Seek(0, SEEK_END); }
	inline off_t GetOffset(void) const { return Seek(0, SEEK_CUR); }
	inline bool Eof(void) const { return (GetOffset() == GetSize()); }
	off_t GetSize(bool = true) const;
	/* directory operations */
	inline bool ForceDirectory(void) const { return m_LocalPath.ForceDirectory(); }
	/* reading (not buffered) */
	int Read(CString * pResult) const;
	int Read(CString * pResult, const int Bytes, const off_t Offset = -1) const;
	int ReadLine(CString * pResult, const off_t Offset = -1) const;
	int ReadLines(CVector<CString> * pResult, bool = false) const;
	unsigned char ReadChar(const off_t Offset = -1) const;
	/* writing (not buffered) */
	bool Write(const CString&, const off_t Offset = -1) const;
	bool WriteLine(const CString&, const off_t Offset = -1) const;
	bool Write(void * pBuffer, const int Bytes) const;
	static bool FileExists(const CString&);
	static long GetFileSize(const CString&);
	static bool GetFileStat(const CString&, struct_stat& Stat);
	static CString GetTmpName(void);
	static int GetTmpDescriptor(CString * pFilename = NULL);
	static bool GetTmpNameSz(char szName[MAXPATHLEN]);
	static int GetTmpDescriptorSz(char szName[MAXPATHLEN]);
    static void GetTmpNameNCS(char szName[MAXPATHLEN]);
#ifdef _WIN32    
    static HANDLE GetTmpHandle(CString * pFilename);
	static HANDLE GetTmpHandleSz(char szName[MAXPATHLEN]); 
#endif
#ifdef _UNIX
	static CString GetTmpPrefix(void);  
#endif
};

#endif
