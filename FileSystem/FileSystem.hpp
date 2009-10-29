/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/


#ifndef BASE_FILE_SYSTEM_HPP
#define BASE_FILE_SYSTEM_HPP

#include <platform/include.hpp>
#include <File/LocalFile.hpp>

class CFileSystem : public CObject {
	property(CLocalPath, Root);
public:
	/* constructors / destructors */
	CFileSystem(void);
	CFileSystem(const CString&); // with a root directory
	virtual ~CFileSystem(void);
public:
	bool ForceDir(const CString& Directory);
	/* UIN - Read or write string */
	int ReadString(const CString& FileName, CString * pResult);
	bool ReadStringBool(const CString& FileName, CString& String);
	void WriteString(const CString& FileName, const CString& String);
};

#endif
