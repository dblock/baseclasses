/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include "FileSystem.hpp"

CFileSystem::CFileSystem(void) {
	m_Root = m_Root.GetCurrentDirectory() + "Data\\";
	m_Root.ForceDirectory();
}

CFileSystem::CFileSystem(const CString& Root) : CObject() {
	m_Root = Root;
	m_Root.ForceDirectory();
}

CFileSystem::~CFileSystem(void) {
}

bool CFileSystem::ForceDir(const CString& Directory) {
	CLocalPath Path(m_Root.GetPath() +  Directory);
	return Path.ForceDirectory();
}

int CFileSystem::ReadString(const CString& FileName, CString * pResult) {
	CLocalFile LocalFile(m_Root.GetFullPath() + FileName);
	LocalFile.ForceDirectory();
	LocalFile.Open(O_RDONLY | O_CREAT);
	return LocalFile.Read(pResult);
}

bool CFileSystem::ReadStringBool(const CString& FileName, CString& String) {
	CLocalFile LocalFile(m_Root.GetFullPath() + FileName);
	LocalFile.ForceDirectory();
	if (LocalFile.Open(O_RDONLY)) {
                LocalFile.Read(&String);
		return true;
	} else return false;
}

void CFileSystem::WriteString(const CString& FileName, const CString& String) {
	CLocalFile LocalFile(m_Root.GetFullPath() + FileName);
	LocalFile.ForceDirectory();
	LocalFile.Open(O_RDWR | O_TRUNC | O_CREAT);
	LocalFile.Write(String);
}
