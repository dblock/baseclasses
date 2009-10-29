/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TEMPLATES_VEC_HPP
#define BASE_TEMPLATES_VEC_HPP

#include <platform/include.hpp>

#ifdef _WIN32

#pragma message( "Using BASE C++ Library" )
#pragma message( "(c) Vestris Inc., Geneva, Switzerland" )
#pragma message( "http://www.vestris.com, 1994-2002 All Rights Reserved" )
#pragma message( "Compile date: " __DATE__ " " __TIME__ )

#pragma warning( push )
#pragma warning( disable : 4660 4700 )
#endif

#include <Vector/Vector.hpp>
#include <Vector/SVector.hpp>

template class CVector<CSVector*>;
template ostream& operator<<(ostream&, CVector<CSVector*> const &);

template class CVector<CSVector>;
template ostream& operator<<(ostream&, CVector<CSVector> const &);

#include <String/String.cpp>
template class CStringTemplate<CSTRING_CHARTYPE>;
#ifdef _UNIX
template ostream& operator<<(ostream&, CString const &);
#endif

template class CVector<CString>;
template ostream& operator<<(ostream&, CVector<CString> const &);

#include <String/StringPair.hpp>
template class CVector<CStringPair>;
template ostream& operator<<(ostream&, CVector<CStringPair> const &);

#include <String/StringTable.hpp>
template class CVector<CStringTable>;
template ostream& operator<<(ostream&, CVector<CStringTable> const &);

#include <Vector/IntVector.hpp>
template class CVector<CIntVector>;
template ostream& operator<<(ostream&, CVector<CIntVector> const &);

typedef CVector<CString> CStringVector;
template class CVector<CStringVector>;
template ostream& operator<<(ostream&, CVector<CStringVector> const &);

#include <Mutex/RWNamedTable.hpp>
template class CVector<CRWNamedMutex *>;

#include <Internet/Url.hpp>
template class CVector<CUrl>;
    
#include <Vector/Vector.cpp>

template class CVector<CObject *>;
template ostream& operator<<(ostream&, CVector<CObject *> const &);

template class CVector<CObject>;
template ostream& operator<<(ostream&, CVector<CObject> const &);

typedef CVector<int> CVectorIntVector;

template class CVector<CVectorIntVector>;
template ostream& operator<<(ostream&, CVector<CVectorIntVector> const &);

template class CVector<int *>;
template ostream& operator<<(ostream&, CVector<int *> const &);

template class CVector<int>;
template ostream& operator<<(ostream&, CVector<int> const &);

template class CVector<long>;
template ostream& operator<<(ostream&, CVector<long> const &);

template class CVector<unsigned int>;
template ostream& operator<<(ostream&, CVector<unsigned int> const &);

template class CVector<unsigned char>;
template ostream& operator<<(ostream&, CVector<unsigned char> const &);

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
