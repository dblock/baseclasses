/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:
    
*/

#ifndef BASE_E_STRINGS_HPP
#define BASE_E_STRINGS_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

bool SSScan(const CString& Source, const CString * const Format, ...);
void ConvertVectorToStr(const CVector<CString>& Vector, const CSTRING_CHARTYPE Separator, CString * pResult);
void ConvertVectorToStr(const CVector<CString>& Vector, const CString& Separator, CString * pResult);
void ConvertStrToVector(const CString& String, const CString& Separator, CVector<CString> * Result);
void ConvertStrToVector(const CString& String, const CSTRING_CHARTYPE Separator, CVector<CString> * Result);

#endif
