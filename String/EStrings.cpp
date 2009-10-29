#include <baseclasses.hpp>
#include "EStrings.hpp"

//#ifndef _WIN32
// const CStringTemplate<CSTRING_CHARTYPE> CStringTemplate<CSTRING_CHARTYPE>::EmptyCString("");
//#endif
 
bool SSScan(const CString& Source, const CString * const Format, ...) {
  assert(Format);

  void * Parameter;
  va_list Options;  
  va_start(Options, Format);
    
  int FormatCurPos = 0;
  int StringCurPos = 0;

  while(1) {

      if (FormatCurPos >= (int) Format->GetLength())
          break;

      switch ((unsigned char) (* Format)[FormatCurPos]) {

      case '\\':
          FormatCurPos++;

          if (FormatCurPos >= (int) Format->GetLength())
              return false;
                           
          if (StringCurPos >= (int) Source.GetLength())
              return false;

          if (Source[StringCurPos] != (* Format)[FormatCurPos])
              return false;

          FormatCurPos++;
          StringCurPos++;

          break;

      case '%':
          FormatCurPos++;

          if (FormatCurPos >= (int) Format->GetLength())
              return false;

          Parameter = va_arg(Options, void *);

          //
          // caller can specify NULL, ReadDigit, ReadString and ReadChar are pointer-safe (dblock)
          // if (! Parameter)
          // return false;
          //

          switch((* Format)[FormatCurPos]) {
          case 'd': 
              if (! Source.ReadDigit(StringCurPos, (int *) Parameter)) 
                  return false; 
              break;
          case 's': 
              if (! Source.ReadString(StringCurPos, (CString *) Parameter)) 
                  return false; 
              break;
          case 'c': 
              if (! Source.ReadChar(StringCurPos, (CSTRING_CHARTYPE *) Parameter)) 
                  return false; 
              break;
          default:          
              return false;
          }

          FormatCurPos++;
          break;
          
      default:
          return false;
      }
  }
  va_end(Options);
  return true;
}

void ConvertVectorToStr(const CVector<CString>& Vector, 
                               const CSTRING_CHARTYPE Separator,
                               CString * pResult) {
    pResult->Empty();
    for (register int i=0;i<(int)Vector.GetSize();i++) {
        if (i) 
            pResult->Append(Separator);
        pResult->Append(Vector[i]);
    }
}

void ConvertVectorToStr(const CVector<CString>& Vector,
                               const CString& Separator, 
                               CString * pResult) {
    pResult->Empty();
    for (register int i=0;i<(int)Vector.GetSize();i++) {
        if (i) 
            pResult->Append(Separator);
        pResult->Append(Vector[i]);
    } 
}

void ConvertStrToVector(const CString& String, 
                               const CString& Separator, 
                               CVector<CString> * Result) {
    Result->RemoveAll();
    int j = 0;
    int i = String.Pos(Separator);
    CString MidString;
    if (String.GetLength()) {
        while (i >= 0) {
            MidString.Empty();
            if (i-j) {
                MidString.CopyBuffer(String.GetBuffer() + j, (int) (i - j));
            }
            Result->Add(MidString);
            j = i + Separator.GetLength();
            i = String.Pos(Separator, j);
        }
        MidString.Empty();
        if (String.GetLength() - j)
            MidString.CopyBuffer(String.GetBuffer() + j, String.GetLength() - j);
        Result->Add(MidString);
    }
}

void ConvertStrToVector(const CString& String, 
                               const CSTRING_CHARTYPE Separator, 
                               CVector<CString> * Result) {
    Result->RemoveAll();
    CString MidString;
    if (String.GetLength()) {          
        int j = 0;
        int i = String.Pos(Separator);
        while (i >= 0) {
            MidString.Empty();
            if (i-j) {
              MidString.CopyBuffer(String.GetBuffer() + j, (int) (i - j));
            }
            Result->Add(MidString);
            j = i + 1;
            i = String.Pos(Separator, j);
        }
        MidString.Empty();
        if (String.GetLength() - j)
            MidString.CopyBuffer(String.GetBuffer() + j, String.GetLength() - j);
        Result->Add(MidString);
    }
}
