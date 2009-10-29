/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    _____________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:
    
    09.09.1999: added Mapping macro/function support
    09.09.1999: IsInt, IsFloat, IsLong support negative values
    11.09.1999: added GetCount(const char Ch)
    23.02.2000: using a stack allocated block for small strings    

*/

#ifndef BASE_STRING_HPP
#define BASE_STRING_HPP

#include <platform/include.hpp>
#include <Vector/Vector.hpp>

// (str) 51763 178163 24057 35240 27112 54402 26797 34015 26273 14771 13615 13759
//       11462 14140  9852  7621  13306 9285  6804  6209  5243  5191  5853  6769 
//                        threshold (18) ^

#define CSTRING_STACK_SIZE 18
#define CSTRING_CHARTYPE char

template <class C>
class CStringTemplate : public CObject {
    BASE_GUARD(m_ClassGuard);    
    C m_StackData[CSTRING_STACK_SIZE];    
    BASE_GUARD(m_StackGuard);
    C * m_RealAllocData;
    BASE_GUARD(m_HeapGuard);
private:
    copy_property(C *, Data);
    readonly_property(unsigned int, Length);
    readonly_property(unsigned int, Size);
    BASE_GUARD(m_PropGuard);
public:
    static const CStringTemplate<C> EmptyCString;
    /* size and length */    
    inline void SetSize(unsigned int Size, bool bPreserve = true);
    inline void SetLength(unsigned int Length, C FillChar = 0, bool bFill = true);
    inline void SetLength(void);
    /* references and constructors */
    inline void CopyBuffer(const C * const Buffer);
    inline void CopyBuffer(const C * const Buffer, int Length);
    inline void Initialize(void);
    CStringTemplate(void);
    CStringTemplate(const C * const);
    CStringTemplate(const C * const, int);
    CStringTemplate(const CStringTemplate<C>&);
    CStringTemplate(const C);
    ~CStringTemplate(void); // not used to be superclassed, was virtual
    /* assignment */
    virtual void operator=(const CStringTemplate<C>&);
    inline virtual void operator=(const C);
    inline virtual void operator=(const C * const);
public:
    /* comparison */
    inline bool operator<(const CStringTemplate<C>& RefString) const { return Smaller(RefString); }
    inline bool operator>(const CStringTemplate<C>& RefString) const { return Bigger(RefString); }
    inline bool operator!=(const CStringTemplate<C>& RefString) const { return Different(RefString); }
    inline bool operator<=(const CStringTemplate<C>& RefString) const { return SmallerOrEqual(RefString); }
    inline bool operator>=(const CStringTemplate<C>& RefString) const { return BiggerOrEqual(RefString); }
    inline bool operator==(const CStringTemplate<C>& RefString) const { return Equal(RefString); }
    inline bool operator<(const C * String) const { return Smaller(String, base_strlen(String)); }
    inline bool operator>(const C * String) const { return Bigger(String, base_strlen(String)); }
    inline bool operator!=(const C * String) const { return Different(String, base_strlen(String)); }
    inline bool operator<=(const C * String) const { return SmallerOrEqual(String, base_strlen(String)); }
    inline bool operator>=(const C * String) const { return BiggerOrEqual(String, base_strlen(String)); }
    inline bool operator==(const C * String) const { return Equal(String); }    
    inline int Compare(const CStringTemplate<C>& RefString, int Start = 0, bool RespectLength = true) const { return Compare(RefString.m_Data, RefString.m_Length, Start, RespectLength); }
    inline int Compare(const C * RefBuffer, int Len, int Start = 0, bool RespectLength = true) const;
    inline int Compare(const C * RefBuffer, int Start = 0, bool RespectLength = true) const { return Compare(RefBuffer, base_strlen(RefBuffer), Start, RespectLength); }
    inline int CompareInsens(const CStringTemplate<C>& RefString, int Start = 0, bool RespectLength = true) const { return CompareInsens(RefString.m_Data, RefString.m_Length, Start, RespectLength); }
    inline int CompareInsens(const C * RefBuffer, int Len, int Start = 0, bool RespectLength = true) const;
    inline int CompareInsens(const C * RefBuffer, int Start = 0, bool RespectLength = true) const { return CompareInsens(RefBuffer, base_strlen(RefBuffer), Start, RespectLength); }
    inline bool SmallerOrEqual(const CStringTemplate<C>& RefString) const { return SmallerOrEqual(RefString.m_Data, RefString.m_Length); }
    inline bool SmallerOrEqual(const C * RefBuffer, int Len) const;
    inline bool SmallerOrEqual(const C * RefBuffer) const { return SmallerOrEqual(RefBuffer, base_strlen(RefBuffer)); }
    inline bool BiggerOrEqual(const CStringTemplate<C>& RefString) const { return BiggerOrEqual(RefString.m_Data, RefString.m_Length); }
    inline bool BiggerOrEqual(const C * RefBuffer, int Len) const;
    inline bool BiggerOrEqual(const C * RefBuffer) const { return BiggerOrEqual(RefBuffer, base_strlen(RefBuffer)); }
    inline bool Bigger(const CStringTemplate<C>& RefString) const { return Bigger(RefString.m_Data, RefString.m_Length); }
    inline bool Bigger(const C * RefBuffer, int Len) const;
    inline bool Bigger(const C * RefBuffer) const { return Bigger(RefBuffer, base_strlen(RefBuffer)); }
    inline bool BiggerInsens(const CStringTemplate<C>& RefString) const { return BiggerInsens(RefString.m_Data, RefString.m_Length); }
    inline bool BiggerInsens(const C * RefBuffer, int Len) const;
    inline bool BiggerInsens(const C * RefBuffer) const { return BiggerInsens(RefBuffer, base_strlen(RefBuffer)); }
    inline bool Smaller(const CStringTemplate<C>& RefString) const { return Smaller(RefString.m_Data, RefString.m_Length); }
    inline bool Smaller(const C * RefBuffer, int Len) const;
    inline bool Smaller(const C * RefBuffer) const { return Smaller(RefBuffer, base_strlen(RefBuffer)); }
    inline bool SmallerInsens(const CStringTemplate<C>& RefString) const { return SmallerInsens(RefString.m_Data, RefString.m_Length); }
    inline bool SmallerInsens(const C * RefBuffer, int Len) const;
    inline bool SmallerInsens(const C * RefBuffer) const { return SmallerInsens(RefBuffer, base_strlen(RefBuffer)); }
    inline bool Equal(const CStringTemplate<C>& RefString) const { return Equal(RefString.m_Data, RefString.m_Length); }
    inline bool Equal(const C * RefBuffer, int Len) const;
    inline bool Equal(const C * RefBuffer) const { return Equal(RefBuffer, base_strlen(RefBuffer)); }
    inline bool Different(const CStringTemplate<C>& RefString) const { return Different(RefString.m_Data, RefString.m_Length); }
    inline bool Different(const C * RefBuffer, int Len) const;
    inline bool Different(const C * RefBuffer) const { return Different(RefBuffer, base_strlen(RefBuffer)); }
    inline bool Same(const CStringTemplate<C>& RefString) const { return Same(RefString.m_Data, RefString.m_Length); }
    inline bool Same(const C * RefBuffer, int Len) const;
    inline bool Same(const C * RefBuffer) const { return Same(RefBuffer, base_strlen(RefBuffer)); }
    inline bool Same(const CStringTemplate<C>& RefString, bool bCaseSensitive) const { return bCaseSensitive ? Equal(RefString) : Same(RefString); }
    inline bool StartsWithSame(const CStringTemplate<C>& RefString) const { return StartsWithSame(RefString.m_Data, RefString.m_Length); }
    inline bool StartsWithSame(const C * RefBuffer, int Len) const;
    inline bool StartsWithSame(const C * RefBuffer) const { return StartsWithSame(RefBuffer, base_strlen(RefBuffer)); }
    inline bool EndsWithSame(const CStringTemplate<C>& RefString) const { return EndsWithSame(RefString.m_Data, RefString.m_Length); }
    inline bool EndsWithSame(const C * RefBuffer, int Len) const;
    inline bool EndsWithSame(const C * RefBuffer) const { return EndsWithSame(RefBuffer, base_strlen(RefBuffer)); }
    inline bool EndsWith(const CStringTemplate<C>& RefString, bool bCaseSensitive) const { return bCaseSensitive ? EndsWith(RefString) : EndsWithSame(RefString); }
    inline bool StartsWith(const CStringTemplate<C>& RefString) const { return StartsWith(RefString.m_Data, RefString.m_Length); }
    inline bool StartsWith(const C * RefBuffer, int Len) const;
    inline bool StartsWith(const C * RefBuffer) const { return StartsWith(RefBuffer, base_strlen(RefBuffer)); }
    inline bool StartsWith(const CStringTemplate<C>& RefString, bool bCaseSensitive) const { return bCaseSensitive ? StartsWith(RefString) : StartsWithSame(RefString); }
    inline bool EndsWith(const CStringTemplate<C>& RefString) const { return EndsWith(RefString.m_Data, RefString.m_Length); }
    inline bool EndsWith(const C * RefBuffer, int Len) const;
    inline bool EndsWith(const C * RefBuffer) const { return EndsWith(RefBuffer, base_strlen(RefBuffer)); }
    /* streaming */
    inline ostream& operator<<(ostream& Stream) const { Stream << m_Data; return Stream; }
    istream& operator>>(istream& Stream);
    /* typecasts */
    //inline operator const C * const () const { return (const C * const) m_Data; }
    inline const C * const GetBuffer(void) const { return (const C * const) m_Data; }
    /* access */
    /*
    inline C& GetAt(const unsigned int Index) { _S_DEBUG(assert(Index < GetLength())); return m_Data[Index];  }
    inline C GetAt(const unsigned int Index) const { _S_DEBUG(assert(Index < GetLength())); return m_Data[Index]; }
    */
    inline const C& GetAt(const unsigned int Index) const { _S_DEBUG(assert(Index < GetLength())); return m_Data[Index];  }
    inline C& operator[](const unsigned int Index) const { _S_DEBUG(assert(Index < GetLength())); return m_Data[Index];  }
    inline C& GetAt(const unsigned int Index) { _S_DEBUG(assert(Index < GetLength())); return m_Data[Index]; }
    inline void SetAt(const unsigned int Index, const C c) { _S_DEBUG(assert(Index < GetLength())); m_Data[Index] = c; }
    /* concatenation (local) */
    inline void Append(const CStringTemplate<C>&);
    inline void Append(const C);
    inline void Append(const C * const, int);
    inline void Append(const C * const);
    /* ops */
    static inline C UCase(const C Ch) { if ((Ch<='z')&&(Ch>='a')) return (C) (Ch - ('a' - 'A')); else return Ch; }
    static inline C LCase(const C Ch) { if ((Ch<='Z')&&(Ch>='A')) return (C) (Ch + ('a' - 'A')); else return Ch; }
    /* move from a string */
    inline void MoveFrom(CStringTemplate<C>&);
    /* empty & delete */
    void Empty(void);
    void Delete(int Start, int Count);
    /* insert */
    inline void Insert(const unsigned int Position, const CStringTemplate<C>&);
    inline void Insert(const unsigned int Position, const C);
    inline void Insert(const unsigned int Position, const C * const Buffer, int BufferLen = -1);
    inline bool Replace(const C Source, const C Target);
    bool Replace(const CStringTemplate<C>&, const CStringTemplate<C>&, bool bCaseSensitive = true);
    inline bool Replace(const C ChLeft, const C ChRight, const C ChTarget);
    inline int GetCount(const C Ch) const;
    inline int GetCount(const C ChLeft, const C ChRight) const;
    /* case */
    inline void UpperCase(void);
    inline void LowerCase(void);
    inline bool HasUpperCase(int Start = 0) const;
    inline bool HasLowerCase(int Start = 0) const;
    inline void Reverse(void);
    inline void TrimLeft(const C ChL = ' ', const C ChR = ' ');
    inline void TrimRight(const C ChL = ' ', const C ChR = ' ');
    inline void Trim32(void) { TrimRight(0, ' '); TrimLeft(0, ' '); }
    inline void Trim(const C Ch = ' ') { TrimRight(Ch, Ch); TrimLeft(Ch, Ch); }
    /* seek C */
    inline int InvPos(const C Ch) const;
    inline int InvPos(const C Ch, const int Start) const;
    inline int Pos(const C Ch, const int Start = 0) const;
    /* seek string */
    inline int Pos(const CStringTemplate<C>& String, const int Start = 0) const;
    inline int SamePos(const CStringTemplate<C>& String, const int Start = 0) const;
    inline int Pos(const C * const Buffer, const int BufLen, const int Start = 0) const;
    inline int SamePos(const C * const Buffer, const int BufLen, const int Start = 0) const;
    inline int Pos(const C * const Buffer, const int Start = 0) const { return Pos(Buffer, base_strlen(Buffer), Start); }
    inline int SamePos(const C * const Buffer, const int Start = 0) const { return SamePos(Buffer, base_strlen(Buffer), Start); }
    /* other stuff */
    inline int GetInt(const int Start, const int Length) const;
    inline bool IsInt(int * Value = NULL) const;
    inline bool IsLong(int Start, int Length, long * Value) const;
	inline bool IsInt(int Start, int Length, int * Value) const;
    inline bool IsFloat(float * Value = NULL) const;
    inline bool IsLong(long * Value = NULL) const;
    inline bool IsHex(int * Value = NULL) const;
    inline bool TerminateWith(const C Ch);
    inline void RemoveDuplicate(const C ChLeft, const C ChRight);
    inline void Remove(const C ChLeft, const C ChRight);
    /* concatenation (local) */
    inline void operator+=(const CStringTemplate<C>& String) { Append(String); }
    inline void operator+=(const C Char) { Append(Char); }
    inline void operator+=(const C * const Buffer) { Append(Buffer); }
    inline void Quote(void) { Trim('\''); Trim('\"'); operator=('\"' + (* this) + '\"'); }
    inline void Dequote(void) { Trim('\''); Trim('\"'); }    
    /* queries */
    inline int Mid(int First, CStringTemplate<C> * pResult) const { return Mid(First, GetLength(), pResult); }
    int Mid(int First, int Count, CStringTemplate<C> * pResult) const;
    inline int Left(int Count, CStringTemplate<C> * pResult) const { return Mid(0, Count, pResult); }
    inline int Right(int Count, CStringTemplate<C> * pResult) const { return Mid(GetLength() - Count, Count, pResult); };
    int ExtractLine(CStringTemplate<C> * pResult);
    int GetLine(CStringTemplate * pResult, int& Pos) const;
    /* helper functions */
    static bool StrToBool(const CStringTemplate<C>&);
    static CStringTemplate BoolToStr(const bool);
    static CStringTemplate LongToStr(long Value, int LeftPad = 0, const int Base = 10);
    static CStringTemplate IntToStr(int Value, int LeftPad = 0, const int Base = 10);
    static CStringTemplate FloatToStr(float Value, int LeftPad = 0, int FracPad = 0, int Base = 10);
    static CStringTemplate DoubleToStr(double Value, int LeftPad = 0, int FracPad = 0, int Base = 10);
    static CStringTemplate BytesToStr(long);
    static CStringTemplate KBytesToStr(long);    
    inline static int StrToHex(const CStringTemplate<C>& String) { int Result; sscanf((const C *) String.GetBuffer(), "%x", &Result); return Result; }
    inline static int StrToInt(const CStringTemplate<C>& String) { if (String.GetLength()) return atoi((const C *) String.GetBuffer()); else return 0; }
    static float StrToFloat(const CStringTemplate<C>& String) { if (String.GetLength()) return (float) atof((const C *) String.GetBuffer()); else return 0; }
    static double StrToDouble(const CStringTemplate<C>& String) { if (String.GetLength()) return (double) atof((const C *) String.GetBuffer()); else return 0; }
    static long StrToLong(const CStringTemplate<C>& String) { if (String.GetLength()) return atol((const C *) String.GetBuffer()); else return 0; }
    /* scanners */
    bool ReadDigit(int& Position, int * Digit) const;
    bool ReadChar(int& Position, C * Char) const;
    bool ReadString(int& Position, CStringTemplate * String) const;
    /* tokenizers */
    static void StrToVector(const CStringTemplate<C>& String, const C Separator, CVector< CStringTemplate<C> > *);
    static void StrToVector(const CStringTemplate<C>& String, const CStringTemplate<C>& Separator, CVector< CStringTemplate<C> > *);
    static void VectorToStr(const CVector< CStringTemplate<C> >& Vector, const C Separator, CStringTemplate<C> *);
    static void VectorToStr(const CVector< CStringTemplate<C> >& Vector, const CStringTemplate<C>& Separator, CStringTemplate<C> *);
    bool InVector(const CVector< CStringTemplate<C> >& Vector, bool CaseSens = true) const;
    /* static map helpers */
    bool MapCommand(CStringTemplate<C>&) const;
    static bool MapCommand(const CVector<CStringTemplate>&, CStringTemplate<C>&);    
    static bool MapTermRegular(const CStringTemplate<C>& Source, int& curPos);    
    static bool MapTermBackslash(const CStringTemplate<C>& Source, CStringTemplate<C>& Target, int& prevPos, int& curPos);
    static bool MapTermFix(CStringTemplate<C>& Term, CStringTemplate<C>& Target, const CStringTemplate<C>& Prefix, const CStringTemplate<C>& Postfix, const CStringTemplate<C>& Elsefix, const CStringTemplate<C>& Cmdfix, C& IdChar, bool ForceQuote);
    static int MapTermGetRegular(const CStringTemplate<C>& String, C& TagChar, int& curPos, int& prevPos);
    static bool MapTermVariable(const CStringTemplate<C>& Source, CStringTemplate<C>& Term, CStringTemplate<C>& Target, int& prevPos, int& curPos, CStringTemplate<C>& Prefix, CStringTemplate<C>& Postfix, CStringTemplate<C>& Elsefix, CStringTemplate<C>& Cmdfix, C& IdChar);
};

#define CSTRING_INTERVAL(_C, _L, _R) ((_C >= _L)&&(_C <= _R))

#define MAP_TERM_MACRO(_Source, _Target, _MapTerm, _MapTermEach, _ForceQuote, __MORE__) \
   CString Term; \
   int curPos = 0, prevPos = 0; \
   CString Prefix, Postfix, Elsefix, Cmdfix, MidString; \
   CSTRING_CHARTYPE TagChar, IdChar; \
   while (curPos < (int) _Source.GetLength()) { \
      if (CString::MapTermBackslash(_Source, _Target, prevPos, curPos)) { \
         continue; \
      } else if (CString::MapTermVariable(_Source, Term, _Target, prevPos, curPos, Prefix, Postfix, Elsefix, Cmdfix, IdChar)) { \
         while (CString::MapTermRegular(_Source, curPos)) { \
            int Regular = CString::MapTermGetRegular(_Source, TagChar, curPos, prevPos); \
            if (TagChar == '~') { _Source.Mid(prevPos,curPos-prevPos-Regular, &MidString); _MapTerm(MidString, Prefix, __MORE__); } \
            else if (TagChar == '#') { _Source.Mid(prevPos,curPos-prevPos-Regular, &MidString); _MapTerm(MidString, Postfix, __MORE__); } \
            else if (TagChar == '^') { _Source.Mid(prevPos,curPos-prevPos-Regular, &MidString); _MapTerm(MidString, Elsefix, __MORE__); } \
            else if (TagChar == '|') { _Source.Mid(prevPos,curPos-prevPos-Regular, &MidString); _MapTerm(MidString, Cmdfix, __MORE__); } \
            prevPos = curPos; \
        } \
        _MapTermEach(Term, __MORE__); \
        CString::MapTermFix(Term, _Target, Prefix, Postfix, Elsefix, Cmdfix, IdChar, _ForceQuote); \
     } else curPos++; \
   } \
   _Source.Mid(prevPos, _Source.GetLength(), &MidString); \
   _Target+=MidString; \
   return _Target;

template <class C> inline ostream& operator<<(ostream& Stream, const CStringTemplate<C>& String) { return String.operator<<(Stream); }
template <class C> inline istream& operator>>(istream& Stream, CStringTemplate<C>& String) { return String.operator>>(Stream); }
/* comparison (friends) */
template <class C> inline bool operator==(const CStringTemplate<C>& Left, const CStringTemplate<C>& Right) { return Left.operator==(Right); }
template <class C> inline bool operator==(const CStringTemplate<C>& Left, const C * const Right) { return Left.operator==(Right); }
template <class C> inline bool operator==(const C * const Left, const CStringTemplate<C>& Right) { return Right.operator==(Left); }
template <class C> inline bool operator!=(const CStringTemplate<C>& Left, const C * const Right) { return Left.operator!=(Right); }
template <class C> inline bool operator!=(const C * const Left, const CStringTemplate<C>& Right) { return Right.operator!=(Left); }
template <class C> inline bool operator<=(const CStringTemplate<C>& Left, const C * const Right) { return Left.operator<=(Right); }
template <class C> inline bool operator<=(const C * const Left, const CStringTemplate<C>& Right) { return !(Right.operator>(Left)); } 
template <class C> inline bool operator>=(const CStringTemplate<C>& Left, const C * const Right) { return Left.operator>=(Right); }
template <class C> inline bool operator>=(const C * const Left, const CStringTemplate<C>& Right) { return !(Right.operator<(Left)); }
template <class C> inline bool operator<(const CStringTemplate<C>& Left, const C * const Right) { return Left.operator<(Right); }
template <class C> inline bool operator<(const C * const Left, const CStringTemplate<C>& Right) { return !(Right.operator>=(Left)); }
template <class C> inline bool operator>(const CStringTemplate<C>& Left, const C * const Right) { return Left.operator>(Right); }
template <class C> inline bool operator>(const C * const Left, const CStringTemplate<C>& Right) { return !(Right.operator<=(Left)); } 

/* concatenation (friends) */
template <class C> inline CStringTemplate<C> operator+(const CStringTemplate<C>& Left, const CStringTemplate<C>& Right) { CStringTemplate<C> Result(Left); Result += Right; return Result; }
template <class C> inline CStringTemplate<C> operator+(const CStringTemplate<C>& Left, const C Right) { CStringTemplate<C> Result(Left); Result += Right; return Result; }
template <class C> inline CStringTemplate<C> operator+(const C Left, const CStringTemplate<C>& Right) { CStringTemplate<C> Result(Left); Result += Right; return Result; }
template <class C> inline CStringTemplate<C> operator+(const CStringTemplate<C>& Left, const C * const Right) { CStringTemplate<C> Result(Left); Result += Right; return Result; }
template <class C> inline CStringTemplate<C> operator+(const C * const Left, const CStringTemplate<C>& Right) { CStringTemplate<C> Result(Left); Result += Right; return Result; }

typedef CStringTemplate<CSTRING_CHARTYPE> CString;
    
#endif
