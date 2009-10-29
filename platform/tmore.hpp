/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TEMPLATES_MORE_HPP
#define BASE_TEMPLATES_MORE_HPP

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4660 4700 )
#endif

#include <platform/include.hpp>

#include <List/List.cpp>
 
#include <String/String.hpp>
template class CList<CString>;
template class CListElement<CString>;
template ostream& operator<<(ostream&, CList<CString> const &);

template CString operator+(const CString& Left, const CString& Right);
template CString operator+(const CString& Left, const CSTRING_CHARTYPE Right); 
template CString operator+(const CSTRING_CHARTYPE Left, const CString& Right); 
template CString operator+(const CString& Left, const CSTRING_CHARTYPE * Right);
template CString operator+(const CSTRING_CHARTYPE * Left, const CString& Right);
template bool operator==(const CString& Left, const CSTRING_CHARTYPE * const Right);
template bool operator==(const CSTRING_CHARTYPE * const Left, const CString& Right); 
template bool operator!=(const CString& Left, const CSTRING_CHARTYPE * const Right); 
template bool operator!=(const CSTRING_CHARTYPE * const Left, const CString& Right);    
template bool operator>=(const CString& Left, const CSTRING_CHARTYPE * const Right); 
template bool operator>=(const CSTRING_CHARTYPE * const Left, const CString& Right);    
template bool operator<=(const CString& Left, const CSTRING_CHARTYPE * const Right); 
template bool operator<=(const CSTRING_CHARTYPE * const Left, const CString& Right);    
template bool operator>(const CString& Left, const CSTRING_CHARTYPE * const Right); 
template bool operator>(const CSTRING_CHARTYPE * const Left, const CString& Right);    
template bool operator<(const CString& Left, const CSTRING_CHARTYPE * const Right); 
template bool operator<(const CSTRING_CHARTYPE * const Left, const CString& Right);    



#include <Thread/ThreadPool.hpp>
template class CList<CThreadPoolJob>;
template class CListElement<CThreadPoolJob>;
template ostream& operator<<(ostream&, CList<CThreadPoolJob> const &);

template class CList<CThreadPoolThread *>;
template class CListElement<CThreadPoolThread *>;
template ostream& operator<<(ostream&, CList<CThreadPoolThread *> const &);

#include <Vector/Vector.hpp>
typedef CVector<CString> CStringVector;

#include <HashTable/HashTable.cpp>
#include <Socket/Dns.hpp>
template class CHashTable<CDnsPeer *>;
template class CVector<CDnsPeer *>;
template class CHashTable<CString>;
template class CHashTable<CObject *>;
template class CHashTable<CStringVector>;
template class CHashTable<unsigned long>;
template class CVector<unsigned long>;

#include <HashTable/VectorTable.cpp>
#include <String/StringTable.hpp>
template class CVectorTable<CStringTable>;
template class CVectorTable<CStringVector>;

#include <Tree/Tree.cpp>
template class CTreeElement<CString>;
template class CTree<CString>;
template class CTreeElement<unsigned char>;
template class CTree<unsigned char>;

#include <Tree/XmlTree.hpp>
template class CTreeElement<CXmlNode>;
template class CTree<CXmlNode>;
#include <Vector/Vector.cpp>
template class CVector<CXmlNode>;
 

/*
  #include <Email/Email.hpp>
  template class CList<CEmail>;
  template class CList<CEmailAttachment>;  
*/

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
