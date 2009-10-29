/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:
    
    10.09.1999: fixed extra separator in path resolver
	18.09.1999: fixed path resultion on different drive (Win32)

*/

#include <baseclasses.hpp>
#include "LocalPath.hpp"
#include <String/GStrings.hpp>

CLocalPath::CLocalPath(const CString& Path) {
#ifdef _WIN32
    m_Drive = 0;
#endif
    SetPath(Path);
}

CLocalPath::CLocalPath(const CLocalPath& Path) {
    (* this) = Path;
}

CLocalPath& CLocalPath::operator=(const CString& Path) {
    SetPath(Path);
    return * this;
}

CLocalPath::CLocalPath(void) {   	
#ifdef _WIN32
	m_Drive = 0;
#endif
}

CLocalPath& CLocalPath::operator=(const CLocalPath& Path) {    
    m_PathFull = Path.m_PathFull;
	m_Path = Path.m_Path;
	m_Filename = Path.m_Filename;
#ifdef _WIN32
	m_Drive = Path.m_Drive;
	m_NetworkServer = Path.m_NetworkServer;
    Trace(tagSystem, levInfo, ("CLocalPath::operator= [%d][%s]", m_Drive, m_NetworkServer.GetBuffer()));
#endif
    Trace(tagSystem, levInfo, ("CLocalPath::operator= [%s][%s][%s]", m_PathFull.GetBuffer(), m_Path.GetBuffer(), m_Filename.GetBuffer()));
	return * this;
}

CLocalPath::CLocalPath(const CString& Path, const CString& Filename) {
    if (Path.GetLength())
        m_PathFull = Path;
    else m_PathFull = GetCurrentDirectory();

    Trace(tagSystem, levInfo, ("CLocalPath::CLocalPath [%s]", m_PathFull.GetBuffer()));
    
    Terminate(m_PathFull);
    m_Path = m_PathFull;
    m_PathFull += Filename;
#ifdef _WIN32
    m_Path.Replace('/','\\');
#endif
#ifdef _UNIX
    m_Path.Replace('\\','/');
#endif
    m_Filename = Filename;

    Trace(tagSystem, levInfo, ("CLocalPath::CLocalPath [%s][%s][%s]", m_PathFull.GetBuffer(), m_Path.GetBuffer(), m_Filename.GetBuffer()));
}

void CLocalPath::ResolvePath(CVector<CString>& Vector) {
   	for (register int i=Vector.GetSize()-1;i>=0;i--) {
		if (Vector[i] == g_strPathParent) {
			for (register int j=i-1;j>=0;j--) {
				if ((Vector[j] != g_strPathParent)&&(Vector[j] != g_strPathCurrent)) {
					Vector[j] = g_strPathCurrent;
					break;
				}
			}
			Vector.RemoveAt(i);
		} else if (Vector[i] == g_strPathCurrent) Vector.RemoveAt(i);
	}
}

void CLocalPath::SetPath(const CString& Path) {  
    m_PathFull = Path;
#ifdef _WIN32
    m_PathFull.Replace('/','\\');
#endif
#ifdef _UNIX
    m_PathFull.Replace('\\','/');
#endif
#ifdef _WIN32 // drive
    if (m_PathFull.Pos(':') == 1) {
        m_Drive = (unsigned char) tolower((unsigned char) m_PathFull[0]);
        m_PathFull.Delete(0, 2);
    }
#endif
    
    Trace(tagSystem, levInfo, ("CLocalPath::SetPath [%s]", m_PathFull.GetBuffer()));

    CString::StrToVector(m_PathFull, PATH_SEPARATOR, &m_PathVector);
    if (m_PathVector.GetSize()) {
        m_Filename = m_PathVector[m_PathVector.GetSize()-1];
        m_PathVector.RemoveAt(m_PathVector.GetSize()-1);
    } else m_Filename.Empty();
        if ((m_PathVector.GetSize() && m_PathVector[0].GetLength()) || (!m_PathVector.GetSize())) {
	   #ifdef _WIN32
		/* 18.09.1999: fix Win32 and different drives */
		int CurrentDrive = _getdrive();
		_chdrive(tolower(m_Drive) - 'a' + 1);
	   #endif
		CVector<CString> CurDirVector;
                GetCurrentDirectoryVector(&CurDirVector);
           #ifdef _WIN32
		if (m_Drive && CurDirVector.GetSize() && IsDrive(CurDirVector[0]))
			CurDirVector.RemoveAt(0);
		_chdrive(CurrentDrive);
           #endif
	   	/* 10.09.1999: fix for the resolver, extra path separator in vector */
		if ((CurDirVector.GetSize() > 1)&&(!CurDirVector[CurDirVector.GetSize()-1].GetLength()))
			CurDirVector.RemoveAt(CurDirVector.GetSize()-1);
		m_PathVector.InsertAt(0, CurDirVector);
	}
#ifdef _WIN32 // network path
	if ((m_PathVector.GetSize() >= 3) && (m_PathVector[0].GetLength() == 0) && (m_PathVector[1].GetLength() == 0)) {
		m_NetworkServer = m_PathVector[2];
		m_PathVector.RemoveAt(0, 3);
	}
#endif	
	ResolvePath(m_PathVector);
	m_Path.Empty();
#ifdef _WIN32 // drive and network name
	if (m_Drive) {
		m_Path = CString(m_Drive) + ':';
		if ((!m_PathVector.GetSize()) || (m_PathVector[0].GetLength())) m_Path+='\\';
	}
	if (m_NetworkServer.GetLength()) {
		m_Path = "\\\\" + m_NetworkServer + '\\';
	}
#endif
	for (register int i=0;i<(int)m_PathVector.GetSize();i++) {
		if (i) m_Path += PATH_SEPARATOR;
		m_Path += m_PathVector[i];
	}
	Terminate(m_Path);

    Trace(tagSystem, levInfo, ("CLocalPath::SetPath [%s][%s][%s]", m_PathFull.GetBuffer(), m_Path.GetBuffer(), m_Filename.GetBuffer()));
}

CString CLocalPath::GetCurrentDirectory(void) {   	
	
    char Buffer[MAXPATHLEN + 1];
    CString Result;    
    char * pCurrentDirectory = NULL;

    if (base_getcwd(Buffer, MAXPATHLEN)) {
        Result = Buffer;
        Terminate(Result);
        Trace(tagSystem, levInfo, ("CLocalPath::GetCurrentDirectory - getcwd(MAXPATHLEN) succeeded [%s]", Result.GetBuffer()));
        return Result;
    }
    
    pCurrentDirectory = base_getcwd(NULL, 0);

    if (pCurrentDirectory) {
        Result = pCurrentDirectory;
        Terminate(Result);
        Trace(tagSystem, levInfo, ("CLocalPath::GetCurrentDirectory - getcwd(NULL) succeeded [%s]", Result.GetBuffer()));
        free(pCurrentDirectory);
        return Result;
    } 
    
#ifdef _UNIX
 #ifdef HAVE_GETWD
    if (getwd(Buffer)) {
        Result = Buffer;
        Terminate(Result);
        Trace(tagSystem, levInfo, ("CLocalPath::GetCurrentDirectory - getwd() succeeded [%s]", Result.GetBuffer()));
        return Result;
    }
 #endif

    pCurrentDirectory = getenv("PWD");
    if (pCurrentDirectory) {
        Result = pCurrentDirectory;
        Terminate(Result);
        Trace(tagSystem, levInfo, ("CLocalPath::GetCurrentDirectory - getcwd(ENV) succeeded [%s]", Result.GetBuffer()));
        return Result;
    }   

#endif

    Trace(tagSystem, levInfo, ("CLocalPath::GetCurrentDirectory - getcwd() failed."));        
    return CString::EmptyCString;
}

bool CLocalPath::DirectoryExists(const CString& Directory) {
  
#ifdef _WIN32
	if (Directory.GetLength() == 2 && Directory[1] == ':')
		return DirectoryExists(Directory + "\\");
#endif

    struct_stat Stat;
    memset(& Stat, 0, sizeof(struct_stat));
    bool bResult = ((base_stat((const char *) Directory.GetBuffer(), &Stat) != -1) &&
#ifdef _WIN32
        ((Stat.st_mode & _S_IFDIR) > 0)
#endif
#ifdef _UNIX
	S_ISDIR(Stat.st_mode)
#endif
	);

    if (! bResult && Directory.GetLength() && Directory[Directory.GetLength() - 1] == PATH_SEPARATOR)
    {
      CString DirectoryCandidate(Directory);
      DirectoryCandidate.Delete(DirectoryCandidate.GetLength() - 1, 1);
      bResult = ((base_stat((const char *) DirectoryCandidate.GetBuffer(), &Stat) != -1) &&
                #ifdef _WIN32
		 ((Stat.st_mode & _S_IFDIR) > 0)
                #endif
                #ifdef _UNIX
		 S_ISDIR(Stat.st_mode)
                #endif
		 );      
    }

    Trace(tagSystem, levInfo, ("CLocalPath::DirectoryExists [%s][%s](%d)", Directory.GetBuffer(), bResult?"yes":"no", Stat.st_mode));

    return bResult;
}

bool CLocalPath::ForceDirectory(void) const {
  	CString CurrentDirectory = GetCurrentDirectory();
#ifdef _WIN32
	// change drive
#endif
	bool Result = ForceDirectory(m_PathVector, false);
	base_chdir((const char *) CurrentDirectory.GetBuffer());
	return Result;
}

bool CLocalPath::ForceDirectory(const CString& Path) {
    CLocalPath LocalPath(Path);
    return LocalPath.ForceDirectory();
}

bool CLocalPath::ForceDirectory(const CVector<CString>& Vector, bool SaveDirectory) {
    CString CurrentDirectory;
    if (SaveDirectory) CurrentDirectory = GetCurrentDirectory();
    
    CString Target;
    CString CurrentPath;
    bool Result = true;
    for (register int i=0;i<(int)Vector.GetSize();i++) {
        Target = CurrentPath + Vector[i]; Terminate(Target);
        if (base_chdir(Target.GetBuffer()) == -1) {
            if (base_mkdir(Target.GetBuffer()) == -1) {
                Result = false;
                break;
            }
        }
        CurrentPath = Target;
    }
    if (SaveDirectory) base_chdir(CurrentDirectory.GetBuffer());
    return Result;
}

void CLocalPath::PopulatePathContents(
 bool Recursive, 
 bool Files, 
 bool Directories, 
 CVector<CString>& Target, 
 const CString& Root) {

	CString CurrentDirectory = GetCurrentDirectory();
	CString CurrentRoot(Root);
	if (!Root.GetLength()) 
        CurrentRoot = CurrentDirectory;
    CurrentRoot.TerminateWith(PATH_SEPARATOR);
#ifdef _UNIX
	DIR * hFile;
	struct dirent * c_file;
	hFile = opendir((const char *) CurrentRoot.GetBuffer());
	while ((c_file = readdir(hFile))) {
		CString NewDirectory = CurrentRoot;
		CString FileName(c_file->d_name);
		NewDirectory.Append(FileName);
		if ((FileName != g_strPathParent)&&(FileName != g_strPathCurrent)) {
			if (!base_chdir(NewDirectory.GetBuffer())) {
				Terminate(NewDirectory);
				if (Directories) Target += NewDirectory;
				if (Recursive) PopulatePathContents(Recursive, Files, Directories, Target, NewDirectory);
			} else if (Files) Target += NewDirectory;
		}
	}
	closedir(hFile);
#endif
#ifdef _WIN32
	CString NewDirectory;
	CString FileName;
	CString NewRoot(CurrentRoot);     
	NewRoot.Append("*.*");
	
	WIN32_FIND_DATA FindData;
	ZeroMemory(& FindData, sizeof(WIN32_FIND_DATA));
	HANDLE hFile;

	hFile = FindFirstFile(
		(const char *) NewRoot.GetBuffer(),
		& FindData);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	do {

		NewDirectory = CurrentRoot;
		FileName = FindData.cFileName;
		NewDirectory.Append(FileName);

		Trace(tagSystem, levInfo, ("CLocalPath::PopulatePathContents [%s]", NewDirectory.GetBuffer()));

		if ((FileName != g_strPathParent) && (FileName != g_strPathCurrent)) 
		{
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				Terminate(NewDirectory);
				if (Directories) 
				{
				  Target += NewDirectory;
				}
				if (Recursive) PopulatePathContents(Recursive, Files, Directories, Target, NewDirectory);

			} 
			else 
			{
			  if (Files) 
			  {
			    Target += NewDirectory;
			  }
			}
		}

	} while( FindNextFile( hFile, & FindData ) != 0);

	FindClose( hFile );
#endif

	base_chdir(CurrentDirectory.GetBuffer());
}

CVector<CString> CLocalPath::GetPathContents(bool Recursive) {
	CVector<CString> Result;
	PopulatePathContents(Recursive, true, true, Result, m_Path);
	return Result;
}

CVector<CString> CLocalPath::GetPathFiles(bool Recursive) {
	CVector<CString> Result;
	PopulatePathContents(Recursive, true, false, Result, m_Path);
	return Result;
}

CVector<CString> CLocalPath::GetPathDirectories(bool Recursive) {
	CVector<CString> Result;
	PopulatePathContents(Recursive, false, true, Result, m_Path);
	return Result;
}

bool CLocalPath::MoveDirectory(const CString& Source, const CString& Target) {
#ifdef _UNIX
  if (!rename(Source.GetBuffer(), Target.GetBuffer()))
    return true;
  else return false;
#endif
#ifdef _WIN32
  return false;
#endif  
}
 
CString CLocalPath::GetRightmostDirname(const CString& String) {
  CString TmpString(String);
  TmpString.Trim(PATH_SEPARATOR);
  int sPos = TmpString.InvPos(PATH_SEPARATOR);
  if (sPos == -1)
    return TmpString;
  TmpString.Delete(0, sPos + 1);
  return TmpString;
}

#ifdef _WIN32
bool CLocalPath::GetFullPath(const CString& Filename, CString * pResult) {
  if (! pResult)
    return false;

  pResult->SetSize(MAXPATHLEN, false);
  
  DWORD nBufferLength = pResult->GetSize() - 1;

  char * lpFilePart = NULL;

  DWORD nPathLength = ::GetFullPathNameA(Filename.GetBuffer(), nBufferLength, pResult->GetData(), & lpFilePart);

  if (nPathLength > nBufferLength) {
    pResult->SetSize(nPathLength + 1);
    nBufferLength = pResult->GetSize() - 1;
    nPathLength = ::GetFullPathNameA(Filename.GetBuffer(), nBufferLength, pResult->GetData(), & lpFilePart);
  }

  if (nPathLength > nBufferLength)
    return false;

  pResult->SetLength();
  
  return true;
}
#endif

CString CLocalPath::ResolveDirectory(const CString& Base, const CString& Relative) {

    CString LocalDirectory(Relative);
	LocalDirectory.Replace('\\', '/');

	static const CString __DotSlash("./");
	static const CString __DotDotSlash("../");	
	static const CString __SlashDotDotSlash("/../");	

	CString CurrentDirectory = Base;
	CurrentDirectory.Replace('\\', '/');
	
    if (CurrentDirectory.GetLength() && (CurrentDirectory[CurrentDirectory.GetLength()-1] == '/')) {
        CurrentDirectory.Delete(CurrentDirectory.GetLength()-1, CurrentDirectory.GetLength());
    }

    while (CurrentDirectory.StartsWith(__DotSlash)) {
        CurrentDirectory.Delete(0, __DotSlash.GetLength());
    }

    while (LocalDirectory.StartsWith(__DotSlash)) {
        LocalDirectory.Delete(0, __DotSlash.GetLength());
    }

	while (LocalDirectory.StartsWith(__DotDotSlash)) {
		int bPos = CurrentDirectory.InvPos('/');
		if (bPos >= 0) {
			CurrentDirectory.Delete(bPos, CurrentDirectory.GetLength());
			LocalDirectory.Delete(0, __DotDotSlash.GetLength());
			while (LocalDirectory.StartsWith(__DotSlash)) 
				LocalDirectory.Delete(0, __DotSlash.GetLength());
		} else break;
	}

	// resolver suppresses forward slash, but server names might have one appended already

	bool sFlag = true;
	
    if (CurrentDirectory.GetLength() && (CurrentDirectory[CurrentDirectory.GetLength()-1] == '/')) {
        sFlag = false;
    } else if (LocalDirectory.GetLength() && (LocalDirectory[0] == '/')) {
        sFlag = false;
    }

    if (sFlag) {
        CurrentDirectory += '/';
    }

    // cout << "resolving [" << CurrentDirectory << "] with [" << LocalDirectory << "]" << endl;
#ifdef _WIN32
    if (CLocalPath::IsDrive(LocalDirectory)) {
        CurrentDirectory.Empty();
    }
#endif
#ifdef _UNIX
    if (LocalDirectory.StartsWith('/')) {
        CurrentDirectory.Empty();
    }
#endif

	CurrentDirectory += LocalDirectory;
	
    // look for eventual ../.. in the returned directory	

	while (1) {
		int dotPos = CurrentDirectory.Pos(__SlashDotDotSlash);
		if (dotPos == -1) break;
		// find the element on the left of the found /../
		int eltPos = (dotPos?dotPos-1:0);
		for (;eltPos >= 0;eltPos--) if (CurrentDirectory[eltPos] == '/') break;
		CurrentDirectory.Delete(eltPos, dotPos + (__SlashDotDotSlash.GetLength() - 1) - eltPos);
	}

	for (int i = CurrentDirectory.GetLength() - 1; i > 0; i--) {
		if (CurrentDirectory[i] == '/' && CurrentDirectory[i - 1] == '/') {
			CurrentDirectory.Delete(i, 1);
		}
	}

	Trace(tagSystem, levInfo, ("CLocalPath::ResolveDirectory [%s] with [%s] -> [%s]", Base.GetBuffer(), Relative.GetBuffer(), CurrentDirectory.GetBuffer()));

	return CurrentDirectory;
}