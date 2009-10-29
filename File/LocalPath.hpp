/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_LOCAL_FILE_PATH_HPP
#define BASE_LOCAL_FILE_PATH_HPP

#include <platform/platform.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>

#ifdef _UNIX
#define PATH_SEPARATOR '/'
#endif

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#endif

class CLocalPath : public CObject {
	readonly_property(CVector<CString>, PathVector);
	readonly_property(CString, PathFull);
	readonly_property(CString, Path);
	readonly_property(CString, Filename);
#ifdef _WIN32
	readonly_property(CString, NetworkServer);
	readonly_property(unsigned char, Drive);
#endif
public:
	/* constructors */
	CLocalPath(void);
	CLocalPath(const CLocalPath&);
	CLocalPath(const CString&);
	CLocalPath(const CString&, const CString&);
	/* assignment */
	CLocalPath& operator=(const CLocalPath&);
	CLocalPath& operator=(const CString&);
	/* operations */
	inline CString GetFullPath(void) const { return m_Path + m_Filename; }
	static CString GetCurrentDirectory(void);
	inline static void GetCurrentDirectoryVector(CVector<CString> * Result) { CString::StrToVector(GetCurrentDirectory(), PATH_SEPARATOR, Result); }
	bool ForceDirectory(void) const;
        static CString GetRightmostDirname(const CString& String);
	inline static CString& Terminate(CString& String) { if (!String.EndsWith(PATH_SEPARATOR)) String+=PATH_SEPARATOR; return String; }
	inline static CString GetTerminated(const CString& String) { if (!String.EndsWith(PATH_SEPARATOR)) return String + PATH_SEPARATOR; else return String; }
private:
	void SetPath(const CString&);
	static void ResolvePath(CVector<CString>&); // resolve relative concatenated paths	
	static bool ForceDirectory(const CVector<CString>&, bool = true); // recursively create a directory, preserve current
public:
	static void PopulatePathContents(bool Recursive, bool Files, bool Directories, CVector<CString>& Target, const CString& Root);
       #ifdef _WIN32
	inline static bool IsDrive(const CString& Drive) { return ((Drive.GetLength() >= 2) && (Drive[1] == ':') && (isalpha(Drive[0]))); }
       #endif
	inline bool ChDir(void) const { return ChDir(m_Path); }
	inline static bool ChDir(const CString& Path) { return (base_chdir((const char *) Path.GetBuffer()) != -1); }
	static bool DirectoryExists(const CString& Directory);
	static bool ForceDirectory(const CString&); // recursively create a directory, force preserve current
	CVector<CString> GetPathContents(bool Recursive = false);
	CVector<CString> GetPathFiles(bool Recursive = false);
	CVector<CString> GetPathDirectories(bool Recursive = false);
	static bool MoveDirectory(const CString& Source, const CString& Target); // recursively delete a directory
#ifdef _WIN32
	static bool GetFullPath(const CString& Filename, CString * pResult);
#endif
	static CString ResolveDirectory(const CString& Base, const CString& Relative);
};

#endif
