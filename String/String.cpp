/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    30.10.1999: URLENCODE and URLDECODE string map operators (I know including Url.hpp is ugly ;)
    17.12.1999: CLEFT and CRIGHT string map operators
    18.03.2000: snprintf instead of sprintf
    20.03.2000: InVector
    09.04.2000: Regexp operator must be isalnum
    09.04.2000: corrected TrimRight/TrimRight swapped (in RefString.cpp)
    01.07.2000: added URLSCH, URLHOST, URLDIR, URLFILE, URLARG string map operators
    11.11.2000: added HTMLQUOTE, HTMLDEQUOTE

*/

#include <baseclasses.hpp>

#include "String.hpp"
#include <Internet/Url.hpp>
#include <Internet/HtmlParser.hpp>
#include <String/GStrings.hpp>
#include <String/EStrings.hpp>

//
// defined in EStrings.hpp because gcc 2.7.2.1 is too dumb to see that it's a static and should
// be defined globally, Win32 issues warnings for the CList template if this is not here (dblock)
//

// #ifdef _WIN32
// CStringTemplate<CSTRING_CHARTYPE> CStringTemplate<CSTRING_CHARTYPE>::EmptyCString("");
// #endif

template <class C>
const CStringTemplate<C> CStringTemplate<C>::EmptyCString;

#ifdef BASE_DEBUG
#define IC_GUARD_INIT                             \
    : BASE_GUARD_INIT(m_ClassGuard)               \
    , BASE_GUARD_INIT(m_StackGuard)               \
    , BASE_GUARD_INIT(m_HeapGuard)                \
    , BASE_GUARD_INIT(m_PropGuard)
#else
#define IC_GUARD_INIT
#endif

template <class C>
CStringTemplate<C>::CStringTemplate(void)
IC_GUARD_INIT
{
    // _L_DEBUG(1, cout << "CStringTemplate<C>::CStringTemplate<C>(void)" << endl);
    Initialize();
}

template <class C>
CStringTemplate<C>::~CStringTemplate(void) {
    // _L_DEBUG(1, cout << "CStringTemplate<C>::~CStringTemplate<C>(void) - [len: " << m_Length << " / size: " << m_Size << " {" << this->GetBuffer() << "} // " << (m_RealAllocData?(C *)m_RealAllocData:"NULL") << "]" << endl);
    if (m_RealAllocData) {
        delete[] m_RealAllocData;
    }
    BASE_GUARD_CHECK(m_ClassGuard);
    BASE_GUARD_CHECK(m_StackGuard);
    BASE_GUARD_CHECK(m_HeapGuard);
    BASE_GUARD_CHECK(m_PropGuard);
}

template <class C>
CStringTemplate<C>::CStringTemplate(const C * const Buffer)
IC_GUARD_INIT
{
    // _L_DEBUG(1, cout << "CStringTemplate<C>::CStringTemplate<C>(const C * const) - " << Buffer << endl);
    Initialize();
    if (Buffer)
        CopyBuffer(Buffer, base_strlen(Buffer));    
}

template <class C>
CStringTemplate<C>::CStringTemplate(const C * const Buffer, int Len)
IC_GUARD_INIT
{
    // _L_DEBUG(1, cout << "CStringTemplate<C>::CStringTemplate<C>(const C * const) - " << Buffer << endl);
    Initialize();
    if (Buffer)
        CopyBuffer(Buffer, Len);
}

template <class C>
CStringTemplate<C>::CStringTemplate(const CStringTemplate<C>& RefString)
IC_GUARD_INIT
{
    Initialize();
    operator=(RefString);
}

template <class C>
CStringTemplate<C>::CStringTemplate(const C Char)
IC_GUARD_INIT
{
    Initialize();
    m_Data[0] = Char;
    m_Data[1] = 0;
    m_Length = 1;
}

template <class C>
istream& CStringTemplate<C>::operator>>(istream& Stream) {
    C Buffer[32], Char;
    while(1) {
        Char = '\n';
        while ((Char == '\n') || (Char == ' ')) {
            Char = (C) Stream.get();
            if ((Char != '\n') && (Char != ' ')) {
                Stream.putback(Char);
                break;
            }
        }
        Stream.getline(Buffer, 31);
    }
    (* this) = Buffer;
    return Stream;
}

template <class C>
int CStringTemplate<C>::Mid(int First, int Count, CStringTemplate<C> * pResult) const {
    pResult->Empty();
    // _L_DEBUG(1, cout << "CStringTemplate::Mid(" << First << "/" << Count << ")" << endl);
    if (First < 0) {
        Count += First;
        First = 0;
    }
    if (First >= (int) GetLength())
        return 0;
    if (Count + First >= (int) GetLength()) 
        Count = GetLength() - First;
    if (Count <= 0) 
        return 0;
    
    pResult->SetLength(Count);
    for (register int i=0;i<Count;i++)
        (* pResult)[i] = operator[](i + First);
    
    return pResult->GetLength();
}

template <class C>
CStringTemplate<C> CStringTemplate<C>::LongToStr(long Value, int LeftPad, const int Base) {
    CStringTemplate<C> Result;
    Result.SetSize(5);
    bool NegativeSign = false;
    
    if (Value < 0) {
        NegativeSign = true;
        Value = -Value;
    }
    
    long Digit;
    do {
        Digit = Value / Base;
        Result.Insert(0, (char) ('0' + Value - (Digit * Base)));
        Value = Digit;
    } while (Value);
    
    if (LeftPad) {
        LeftPad -= ((int) NegativeSign + Result.GetLength());
        while (LeftPad--) Result.Insert(0, '0');
    }
    
    if (NegativeSign) 
        Result.Insert(0, '-');
    return Result;
}


template <class C>
CStringTemplate<C> CStringTemplate<C>::IntToStr(int Value, int LeftPad, int Base) { 
    return LongToStr(Value, LeftPad, Base); 
}


template <class C>
CStringTemplate<C> CStringTemplate<C>::DoubleToStr(double Value, int LeftPad, int FracPad, int Base) {
    long Truncated = (long) Value;
    double Fraction = Value - Truncated;
    if (Fraction < 0) Fraction = - Fraction;
    
    CStringTemplate<C> FractionString; 
    FractionString.SetSize(5);
    while (Fraction) {
        Fraction *= (double) Base;		
        FractionString += ('0' + (char) (long) Fraction);
        Fraction -= ((long) Fraction);		
    }
    
    if (FractionString.GetLength() > 5)
        FractionString.Delete(5, FractionString.GetLength());
    
    FractionString.TrimRight('0', '0');
    
    int AppendZeros =  FracPad - (int) FractionString.GetLength();
    while (AppendZeros-- > 0) FractionString += '0';
    
    if ((FracPad && ((int) FractionString.GetLength() > FracPad)))
        FractionString.Delete(FracPad, FractionString.GetLength());
    
    CStringTemplate<C> Result = LongToStr(Truncated, LeftPad, Base);
    if (FractionString.GetLength()) {
        Result += '.';
        Result += FractionString;
    }
    return Result;
}


template <class C>
CStringTemplate<C> CStringTemplate<C>::FloatToStr(float Value, int LeftPad, int FracPad, int Base) { 
    return DoubleToStr(Value, LeftPad, FracPad, Base); 
}

template <class C>
bool CStringTemplate<C>::InVector(const CVector< CStringTemplate<C> >& Vector, bool CaseSens) const {
    if (CaseSens) return Vector.Contains(* this);
    else for (register int i=0;i<(int)Vector.GetSize();i++)
        if (Same(Vector[i])) return true;
        return false;
}


template <class C>
CStringTemplate<C> CStringTemplate<C>::BytesToStr(const long Bytes) {	
    if (Bytes > GBYTE) return FloatToStr(((float)Bytes)/GBYTE, 0, 1) + g_strSGb;
    else if (Bytes > MBYTE) return FloatToStr(((float)Bytes)/MBYTE, 0, 1) + g_strSMb;
    else if (Bytes > KBYTE) return FloatToStr(((float)Bytes)/KBYTE, 0, 1) + g_strSKb;
    else return LongToStr(Bytes) + " Bytes";	
}


template <class C>
CStringTemplate<C> CStringTemplate<C>::KBytesToStr(const long KBytes) {	
    if (KBytes > MBYTE) return FloatToStr(((float)KBytes)/MBYTE, 0, 1) + g_strSGb;
    else if (KBytes > KBYTE) return FloatToStr(((float)KBytes)/KBYTE, 0, 1) + g_strSMb;
    else return LongToStr(KBytes) + g_strSKb;	
    return CStringTemplate<C>::EmptyCString;
}

template <class C>
bool CStringTemplate<C>::StrToBool(const CStringTemplate<C>& String) {
  if (String.Same(g_strTrue)) return true;
  else if (String.Same(g_strFalse)) return false;
  else if (String.Same(g_strOne)) return true;
  else if (String.Same(g_strZero)) return false;
  else if (String.Same(g_strYes)) return true;
  else if (String.Same(g_strNo)) return false;
  else if (String.Same(g_strY)) return true;
  else if (String.Same(g_strN)) return false;
  else return (String.GetLength() > 0);
}

template <class C>
CStringTemplate<C> CStringTemplate<C>::BoolToStr(const bool Value) {
    return ((Value)?g_strTrue:g_strFalse);
}

template <class C>
int CStringTemplate<C>::GetLine(CStringTemplate<C> * pResult, int& Pos) const {	
    pResult->Empty();
    while (Pos < (int) GetLength()) {
        if (GetAt(Pos) == 10) {
            Pos++;
            break;
        } else if (GetAt(Pos) != 13) {
            pResult->Append(GetAt(Pos));
        }
        Pos++;
    }
    return pResult->GetLength();
}

template <class C>
int CStringTemplate<C>::ExtractLine(CStringTemplate<C> * pResult) {
    pResult->Empty();
    int CurPos = 0;	
    while (CurPos < (int) GetLength()) {
        if (GetAt(CurPos) == 10) {
            CurPos++;
            break;
        } else if (GetAt(CurPos) != 13) {			
            pResult->Append(GetAt(CurPos));
        }
        CurPos++;
    }
    if (CurPos != 0)
        Delete(0, CurPos);
    return pResult->GetLength();
}

template <class C>
bool CStringTemplate<C>::ReadDigit(int& Position, int * Digit) const {
    while ((Position < (int) GetLength())&&(operator[](Position) <= ' ')) Position++;
    int Result = 0; bool Valid = false;
    while (Position < (int) GetLength()) {
        if (isdigit(operator[](Position))) {
            Valid = true;
            Result *= 10;
            Result += (operator[](Position) - '0');
            Position++;
        } else break;
    }
    // _L_DEBUG(1, cout << "CStringTemplate::Scan() - Read digit: [" << Result << "]" << endl);
    if (Digit) * Digit = Result;
    // _L_DEBUG(1, if (!Valid) cout << "CStringTemplate::Scan() - Expected integer at " << Position << endl);
    return Valid;
}


template <class C>
bool CStringTemplate<C>::ReadChar(int& Position, C * Char) const {
    while ((Position < (int) GetLength())&&(operator[](Position) <= ' ')) Position++;
    if (Position < (int) GetLength()) {
        // _L_DEBUG(1, cout << "CStringTemplate::Scan() - Read char: [" << operator[](Position) << "]" << endl);
        if (Char) * Char = operator[](Position);
        Position++;
        return true;
    } else {
        // _L_DEBUG(1, cout << "CStringTemplate::Scan() - Expected character at " << Position << endl);
        return false;
    }
}


template <class C>
bool CStringTemplate<C>::ReadString(int& Position, CStringTemplate<C> * pString) const {
    while ((Position < (int) GetLength())&&(operator[](Position) <= ' ')) Position++;
    int PrevPos = Position;
    while ((Position < (int) GetLength())&&(isalpha(operator[](Position)))) Position++;
    if (PrevPos != Position) {
        if (pString)
        {
            Mid(PrevPos, Position - PrevPos, pString);
            // _L_DEBUG(1, cout << "CStringTemplate::Scan() - Read string: [" << (* pString) << "]" << endl);
        }
        return true;
    } else {
        // _L_DEBUG(1, cout << "CStringTemplate::Scan() - Expected String at " << Position << endl);
        return false;
    }
}


template <class C>
bool CStringTemplate<C>::Replace(const CStringTemplate<C>& Source, const CStringTemplate<C> & Target, bool bCaseSensitive) {
	bool bResult = false;
    int curPos = 0;
	while ((curPos = bCaseSensitive ? Pos(Source, curPos) : SamePos(Source, curPos)) != -1) {
        Delete(curPos, Source.GetLength());
        Insert(curPos, Target);
        curPos+=Target.GetLength();
		bResult = true;
    }
	return bResult;
}

template <class C>
bool CStringTemplate<C>::MapCommand(CStringTemplate<C>& Term) const {
    if (Same(g_strCMD_TRIM)) Term.Trim();
    else if (Same(g_strCMD_TRIM32)) Term.Trim32();
    else if (Same(g_strCMD_UPCASE)) Term.UpperCase();
    else if (Same(g_strCMD_LCASE)) Term.LowerCase();
    else if (Same(g_strCMD_REVERSE)) Term.Reverse();
    else if (Same(g_strCMD_URLENCODE)) Term = CUrl::Escape(Term);
    else if (Same(g_strCMD_URLDECODE)) Term = CUrl::UnEscape(Term);
    else if (Same(g_strCMD_HTMLQUOTE)) CHtmlParser::Quote(Term);
    else if (Same(g_strCMD_HTMLDEQUOTE)) CHtmlParser::TranslateQuotes(Term);
	else if (StartsWithSame(g_strCMD_REPLACE)) {
		CStringTemplate<C> Op;
		Mid(base_strlen(g_strCMD_REPLACE), GetLength(), & Op);		
		int sPos = Op.Pos(':');
		if (sPos >= 0) {
			CStringTemplate<C> Left, Right;			
			Op.Mid(0, sPos, & Left);
			Op.Mid(sPos + 1, GetLength(), & Right);
			Term.Replace(Left, Right);
		}
	} else if (StartsWithSame(g_strCMD_REPLACECI)) {
		CStringTemplate<C> Op;
		Mid(base_strlen(g_strCMD_REPLACE), GetLength(), & Op);		
		int sPos = Op.Pos(':');
		if (sPos >= 0) {
			CStringTemplate<C> Left, Right;			
			Op.Mid(0, sPos, & Left);
			Op.Mid(sPos + 1, GetLength(), & Right);
			Term.Replace(Left, Right, false);
		}
	} else if (StartsWithSame(g_strCMD_CLEFT)) {
        int Length; 
        if ((IsInt(base_strlen(g_strCMD_CLEFT), GetLength(), &Length))&&((int) Term.GetLength() > Length))
            Term.Delete(0, Length);
    } else if (StartsWithSame(g_strCMD_CRIGHT)) {
        int Length;
        if ((IsInt(base_strlen(g_strCMD_CRIGHT), GetLength(), &Length))&&((int) Term.GetLength() > Length))
            Term.Delete(Term.GetLength() - Length, Term.GetLength());
    } else if (StartsWithSame(g_strCMD_LEFT)) {
        int MaxLength;
        if ((IsInt(base_strlen(g_strCMD_LEFT), GetLength(), &MaxLength))&&((int) Term.GetLength() > MaxLength))
            Term.Delete(MaxLength, Term.GetLength());
    } else if (StartsWithSame(g_strCMD_RIGHT)) {
        int MaxLength;
        if ((IsInt(base_strlen(g_strCMD_RIGHT), GetLength(), &MaxLength))&&((int) Term.GetLength() > MaxLength))
            Term.Delete(0, Term.GetLength() - MaxLength);
    } else if (StartsWithSame(g_strCMD_IS)) {
        if (Term.Different(GetBuffer() + base_strlen(g_strCMD_IS), GetLength() - base_strlen(g_strCMD_IS)))
            Term.Empty();
    } else if (StartsWithSame(g_strCMD_NOT)) {
        if (Term.Equal(GetBuffer() + base_strlen(g_strCMD_NOT), GetLength() - base_strlen(g_strCMD_NOT)))
            Term.Empty();
    } else if (StartsWithSame(g_strCMD_HAS)) {
        CStringTemplate<C> MidString;
        Mid(base_strlen(g_strCMD_HAS), GetLength(), &MidString);
        if (Term.Pos(MidString) == -1) 
            Term.Empty();
    } else if (StartsWithSame(g_strCMD_STARTS)) {
        if (!Term.StartsWithSame(GetBuffer() + base_strlen(g_strCMD_STARTS), GetLength() - base_strlen(g_strCMD_STARTS))) 
            Term.Empty();
    } else if (StartsWithSame(g_strCMD_ENDS)) {
        if (!Term.EndsWithSame(GetBuffer() + base_strlen(g_strCMD_ENDS), GetLength() - base_strlen(g_strCMD_ENDS))) 
            Term.Empty();
    } else if (StartsWithSame(g_strCMD_MORE)) {
        int Value;
        if (IsInt(base_strlen(g_strCMD_MORE), GetLength(), &Value))
            if (CStringTemplate::StrToInt(Term) <= Value) 
                Term.Empty();
    } else if (StartsWithSame(g_strCMD_LESS)) {
        int Value;
        if (IsInt(base_strlen(g_strCMD_LESS), GetLength(), &Value))
            if (CStringTemplate::StrToInt(Term) >= Value) 
                Term.Empty();
    } else if (Same(g_strCMD_URLSCH)) {
        Term = CUrl(Term).GetScheme();
    } else if (Same(g_strCMD_URLHOST)) {
        Term = CUrl(Term).GetHost();
    } else if (Same(g_strCMD_URLDIR)) {
        Term = CUrl(Term).GetHttpDirectory();
    } else if (Same(g_strCMD_URLFILE)) {
        Term = CUrl(Term).GetHttpFilename();
    } else if (Same(g_strCMD_URLARG)) {
        Term = CUrl(Term).GetHttpSearchPath();
    } else return false;
    return true;
}

template <class C>
bool CStringTemplate<C>::MapTermBackslash(
    const CStringTemplate<C>& Source, 
    CStringTemplate<C>& Target, 
    int& prevPos, 
    int& curPos) {

    if ((curPos < ((int) Source.GetLength()) - 2) && (Source[curPos] == '\\')) {
        Target.Append(Source.GetBuffer() + prevPos, curPos - prevPos);
        curPos++;
        Target+=Source[curPos];
        curPos++;
        prevPos=curPos;        
        return true;
    } else return false;
}

template <class C>
bool CStringTemplate<C>::MapTermFix(
    CStringTemplate<C>& Term,
    CStringTemplate<C>& Target,
    const CStringTemplate<C>& Prefix, 
    const CStringTemplate<C>& Postfix,
    const CStringTemplate<C>& Elsefix, 
    const CStringTemplate<C>& Cmdfix,
    C& IdChar,
    bool ForceQuote) {

    if (Cmdfix.GetLength()) {
        CVector<CString> Vector;
        CString::StrToVector(Cmdfix,',', &Vector);
        CString::MapCommand(Vector, Term);	
    }

    // cout << "Term:[" << Term << "] Prefix:[" << Prefix << "] Postfix:[" << Postfix << "] Elsefix: [" << Elsefix << "] Cmdfix: [" << Cmdfix << "]" << endl;

    if (Term.GetLength()) {
        
        Target+=Prefix;

        if (ForceQuote) {
            CHtmlParser::Quote(CString(Term), &Term);
        }

        if (IdChar != '£') {
            Target+=Term;
        }

        Target+=Postfix;

    } else {
        Target+=Elsefix;
    }

    return true;
}

template <class C>
bool CStringTemplate<C>::MapTermRegular(
    const CStringTemplate<C>& Source, int& curPos) {

    return (
        (curPos < (int) Source.GetLength()) &&
        (
         (Source[curPos] == '~') || 
         (Source[curPos] == '#') ||
         (Source[curPos] == '^') ||
         (Source[curPos] == '|')
        )
       );
}


template <class C>
bool CStringTemplate<C>::MapCommand(
    const CVector< CStringTemplate<C> >& Commands, 
    CStringTemplate<C>& Term) {

    bool Result = false;
    
    for (register int i=0;i<(int)Commands.GetSize();i++) {        
        Result |= Commands[i].MapCommand(Term);        
    }

    return Result;
}


template <class C>
int CStringTemplate<C>::MapTermGetRegular(
    const CStringTemplate<C>& String,
    C& TagChar,
    int& curPos, 
    int& prevPos) {

    int Regular;
    
    TagChar = String[curPos];
    
    if ((curPos < ((int) String.GetLength()) - 1) && (String[curPos+1] == '[')) {
        Regular = 1; 
    } else {
        Regular = 0;
    }

    curPos++; 
    
    prevPos = curPos;
    
    if (Regular) {
        
        prevPos++;
        Regular++;

        while (curPos < (int) String.GetLength()) {
            
            if (String[curPos] == '\\') {
                if (curPos + 1 < (int) String.GetLength())
                    if (String[curPos + 1] != '\\')
                        curPos++;
            }

            curPos++;

            if (String[curPos] == '[') {

                Regular++;

            } else {
                
                if (String[curPos] == ']') {
                    Regular--;
                }

                if (Regular == 1) 
                    break;
            }
        }

        if ((curPos < (int) String.GetLength()) && (String[curPos] == ']')) 
            curPos++;

    } else {

        while ((curPos < (int) String.GetLength()) &&
            isalnum((unsigned CSTRING_CHARTYPE) String[curPos]))
            curPos++;
    }

    return Regular?1:0;	
}

template <class C>
bool CStringTemplate<C>::MapTermVariable(
    const CStringTemplate<C>& Source, 
    CStringTemplate<C>& Term, 
    CStringTemplate<C>& Target, 
    int& prevPos, 
    int& curPos,
    CStringTemplate<C>& Prefix, 
    CStringTemplate<C>& Postfix, 
    CStringTemplate<C>& Elsefix, 
    CStringTemplate<C>& Cmdfix,
    C& IdChar) {
    
    if (curPos >= (int) Source.GetLength())
        return false;

    if ((((CSTRING_CHARTYPE) Source[curPos]) == '$')||(((CSTRING_CHARTYPE) Source[curPos]) == '£')) {
        IdChar = Source[curPos];
        Prefix.Empty();
        Postfix.Empty();
        Elsefix.Empty();
        Cmdfix.Empty();
        Target.Append(Source.GetBuffer() + prevPos, curPos - prevPos);
        curPos++;
        bool InBlock = false;

        prevPos = curPos;        
        
        while (curPos < (int) Source.GetLength()) {

            if ((prevPos == curPos) && (Source[curPos] == '[')) {
                InBlock = true;
            } else if (Source[curPos] == ']') {
                // not an alphanum anyway
                if (InBlock)
                    curPos++;
                break;            
            } else if (
                (! InBlock) &&
                (((CSTRING_CHARTYPE) Source[curPos] == '$') ||
                ((CSTRING_CHARTYPE) Source[curPos] == '£'))) {
                break;                
            } else if (
                (! InBlock) &&
                (! isalnum((unsigned CSTRING_CHARTYPE) Source[curPos])) &&
                ((CSTRING_CHARTYPE) Source[curPos] != '_') && 
                ((CSTRING_CHARTYPE) Source[curPos] != '.')) {
                break;
            }

            curPos++;
        }
        
        Source.Mid(
            prevPos + (InBlock ? 1 : 0), 
            curPos - prevPos - (InBlock ? 2 : 0), 
            & Term);
        
        while (
            Term.GetLength() && 
            (((CSTRING_CHARTYPE) Term[Term.GetLength()-1] == '.') ||
             ((CSTRING_CHARTYPE)Term[Term.GetLength()-1] == '_'))) {
            curPos--;
            Term.Delete(Term.GetLength() - 1, 1);
        }
        
        prevPos = curPos;
        return true;
    } else return false;
}

template <class C>
inline void CStringTemplate<C>::StrToVector(const CStringTemplate<C>& String, const C Separator, CVector< CStringTemplate<C> > * pResult) {
    ConvertStrToVector(String, Separator, pResult); 
}

template <class C>
inline void CStringTemplate<C>::StrToVector(const CStringTemplate<C>& String, const CStringTemplate<C>& Separator, CVector< CStringTemplate<C> > * pResult) {
    ConvertStrToVector(String, Separator, pResult); 
}

template <class C>
inline void CStringTemplate<C>::VectorToStr(const CVector< CStringTemplate<C> >& Vector, const C Separator, CStringTemplate<C> * pResult) {
    ConvertVectorToStr(Vector, Separator, pResult); 
}

template <class C>
inline void CStringTemplate<C>::VectorToStr(const CVector< CStringTemplate<C> >& Vector, const CStringTemplate<C>& Separator, CStringTemplate<C> * pResult) {
    ConvertVectorToStr(Vector, Separator, pResult);
}

template <class C>
inline bool CStringTemplate<C>::IsInt(int Pos, int Length, int * pValue) const {
	long lResult = 0;
	if (IsLong(Pos, Length, & lResult)) {
		if (pValue) {
			* pValue = (int) lResult;
		}
		return true;
	}
	return false;
}

template <class C>
inline bool CStringTemplate<C>::IsLong(int Pos, int Length, long * pValue) const {
    int Sign = 1;    
	long NewValue = 0;

    if (Pos + Length > (int) m_Length)
      Length = (int) m_Length - Pos;
    
	if (Pos >= (int) m_Length) 
      return false;

	if (m_Data[Pos] == '-') {
		Pos++;
		Length--;
		Sign = -1; 
	} else if (m_Data[Pos] == '+') {
		Pos++;
		Length--;
	}

	if (Pos >= (int) m_Length) 
        return false;

    while (Length--) {
		
		if (! isdigit(m_Data[Pos])) {
			return false;
		}
      
		NewValue *= 10;
		NewValue += (int) (m_Data[Pos] - '0');
				
		Pos++;
    }

    NewValue *= Sign;
    
	if (pValue) {
        (* pValue) = NewValue;
	}

    return true;
} 

template <class C>
inline void CStringTemplate<C>::RemoveDuplicate(const C ChLeft, const C ChRight) {
  if (m_Length <= 1)
    return;
  
  int l2 = 0;
  for (register int l1 = 0; l1 < (int) m_Length - 1; l1++) {
    if (!(CSTRING_INTERVAL(m_Data[l1], ChLeft, ChRight) &&
          CSTRING_INTERVAL(m_Data[l1 + 1], ChLeft, ChRight))) {
      m_Data[l2] = m_Data[l1];
      l2++;
    }
  }
  m_Data[l2] = m_Data[m_Length - 1];
  m_Length = l2 + 1;
  m_Data[m_Length] = 0;
}

template <class C>
inline void CStringTemplate<C>::Remove(const C ChLeft, const C ChRight) {
  int l2 = 0;
  for (register int l1 = 0; l1 < (int) m_Length; l1++) {
    if (!CSTRING_INTERVAL(m_Data[l1], ChLeft, ChRight)) {
      m_Data[l2] = m_Data[l1];
      l2++;
    }
  }
  m_Length = l2;
  m_Data[m_Length] = 0;
}

template <class C>
inline bool CStringTemplate<C>::TerminateWith(const C Ch) {
    if (!m_Length || m_Data[m_Length - 1] != Ch) {
          Append(Ch);
          return true;
        } else return false;
}

template <class C>
inline int CStringTemplate<C>::GetInt(const int Start, const int Length) const {
    int Result = 0;
	IsInt(Start, Length, & Result);
	return Result;
}
 
template <class C>
inline void CStringTemplate<C>::UpperCase(void) {
    for (register int i=0;i<(int) m_Length;i++)
        if ((m_Data[i]<='z')&&(m_Data[i]>='a'))
            m_Data[i]-=('a' - 'A');
}

template <class C>
inline void CStringTemplate<C>::LowerCase(void) {
    for (register int i=0;i<(int) m_Length;i++)
        if ((m_Data[i]<='Z')&&(m_Data[i]>='A'))
            m_Data[i]+=('a' - 'A');
}

template <class C>
inline bool CStringTemplate<C>::HasUpperCase(int Start) const {
    for (register int i=Start;i<(int) m_Length;i++)
        if ((m_Data[i]<='Z')&&(m_Data[i]>='A'))
            return true;
    return false;
}

template <class C>
inline bool CStringTemplate<C>::HasLowerCase(int Start) const {
    for (register int i=Start;i<(int) m_Length;i++)
        if ((m_Data[i]<='z')&&(m_Data[i]>='a'))
            return true;
    return false;
}

template <class C>
inline void CStringTemplate<C>::Reverse(void) {
    C ch;
    for (register int i=0;i<((int)m_Length)/2;i++) {
        ch = m_Data[i];
        m_Data[i] = m_Data[m_Length-1-i];
        m_Data[m_Length-1-i] = ch;
    }
}

template <class C>
inline int CStringTemplate<C>::InvPos(const C Ch) const {
    return InvPos(Ch, ((int)m_Length)-1);
}

template <class C>
inline int CStringTemplate<C>::InvPos(const C Ch, const int Start) const {
    for (register int i=Start;i>=0;i--)
        if (m_Data[i] == Ch)
            return i;
    return -1;
}

template <class C>
inline int CStringTemplate<C>::Pos(const C Ch, const int Start) const {
    for (register int i=Start;i<((int)m_Length);i++)
        if (m_Data[i] == Ch)
            return i;
    return -1;
}

template <class C>
inline int CStringTemplate<C>::Pos(const CStringTemplate<C>& String, const int Start) const {
    for (register int i=Start;i<((int)m_Length) - ((int)String.m_Length) + 1;i++)
        if (!(memcmp(m_Data + i, String.m_Data, String.m_Length)))
            return i;
    return -1;
}

template <class C>
inline int CStringTemplate<C>::SamePos(const CStringTemplate<C>& String, const int Start) const {
  for (register int i=Start;i<(((int)m_Length) - ((int)String.m_Length) + 1);i++)
    if (CompareInsens(String, i, false) == 0)
      return i;
  return -1;
}

template <class C>
inline bool CStringTemplate<C>::Replace(const C ChLeft, const C ChRight, const C ChTarget) {
	bool bResult = false;
	for (register int i=0;i<(int)m_Length;i++) {
		if (CSTRING_INTERVAL(m_Data[i], ChLeft, ChRight)) {
			m_Data[i] = ChTarget;
			bResult = true;
		}
    }
	return bResult;
}
 
template <class C>
inline bool CStringTemplate<C>::Replace(const C Source, const C Target) {
	bool bResult = false;
    for (register int i=0;i<(int)m_Length;i++) {
		if (m_Data[i] == Source) {
			m_Data[i] = Target;
			bResult = true;
		}
    }
	return bResult;
}

template <class C>
inline int CStringTemplate<C>::GetCount(const C Ch) const {
    int Result = 0;
    for (register int i=0;i<(int)m_Length;i++) {
          if (m_Data[i] == Ch) 
            Result++;
    }
    return Result;
}

template <class C>
inline int CStringTemplate<C>::GetCount(const C ChLeft, const C ChRight) const {
    int Result = 0;
    for (register int i=0;i<(int)m_Length;i++) {
          if (CSTRING_INTERVAL(m_Data[i], ChLeft, ChRight))
            Result++;
    }
    return Result;
}

template <class C>
inline bool CStringTemplate<C>::IsInt(int * Value) const {
	return IsInt(0, m_Length, Value);    
}

template <class C>
inline bool CStringTemplate<C>::IsHex(int * Value) const {
    if (!m_Length) return false;
    for (register int i=0;i<(int)m_Length;i++) {
        if (
            (!isdigit(m_Data[i])) &&
            ((m_Data[i] > 'F')||(m_Data[i] < 'A')) &&
            ((m_Data[i] > 'f')||(m_Data[i] < 'a'))
            ) return false;
    }
    if (Value) {
		// this needs to be rewritten
        sscanf((const C *) m_Data, "%x", Value);
    }
    return true;
}

template <class C>
inline bool CStringTemplate<C>::IsFloat(float * Value) const {    
    bool Separator = false;
    register int i=0, Sign = 1, Displ = 0;	
    if (m_Length && (m_Data[0]=='-')) { i++; Displ++; Sign=-1; }
    if (i == (int) m_Length) return false;
    for (;i<(int)m_Length;i++)
        if (!isdigit(m_Data[i])) {
            if (Separator) return false;
            else if (m_Data[i] == '.') Separator = true;
            else return false;
        }
    if (Value) (* Value) = (float) (atof((const C *) m_Data + Displ) * Sign);
    return true;
    
}

template <class C>
inline bool CStringTemplate<C>::IsLong(long * Value) const {
	return IsLong(0, m_Length, Value);
}

template <class C>
inline int CStringTemplate<C>::Pos(const C * const Buffer, const int Len, const int Start) const {
  int Length = (int) m_Length - Len + 1;
  for (register int i = Start; i < Length; i++)
    if (Compare(Buffer, Len, i, false) == 0)
      return i;
  return -1;
}

template <class C>
inline int CStringTemplate<C>::SamePos(const C * const Buffer, const int Len, const int Start) const {
  int Length = (int) m_Length - Len + 1;
  for (register int i = Start; i < Length; i++)
    if (CompareInsens(Buffer, Len, i, false) == 0)
      return i;
  return -1;
}

template <class C>
inline void CStringTemplate<C>::Append(const C * const Buffer) {
    if (Buffer)
        Append(Buffer, base_strlen(Buffer));
}

template <class C>
inline void CStringTemplate<C>::SetSize(unsigned int DesiredSize, bool bPreserve) {
  if (DesiredSize <= m_Size)
    return;
  
  // Size = ((Size + CSTRING_STACK_SIZE - 1) / CSTRING_STACK_SIZE) * CSTRING_STACK_SIZE;

  unsigned int Size = DesiredSize;
  if (Size < MBYTE) {
    Size = (m_Size << 1) + 1;
    while ( Size < DesiredSize)
      Size <<= 1;
  } else {
    Size = ((Size + KBYTE - 1) / KBYTE) * KBYTE;
  }

  C * m_NewData = new C[Size];
 
  if (bPreserve && m_Length) {
    memcpy(m_NewData, m_Data, m_Length * sizeof(C));
    m_NewData[m_Length] = 0;
  }
  if (m_RealAllocData) {
    delete[] m_RealAllocData;
  }
  m_RealAllocData = m_NewData;
  m_Data = m_NewData;
  m_Size = Size;
}

/* concatenation (local) */
template <class C>
inline void CStringTemplate<C>::Append(const CStringTemplate<C>& String) {
  SetSize(m_Length + String.m_Length + 1, true);
  memcpy(m_Data + m_Length, String.m_Data, String.m_Length);
  m_Length += String.m_Length;
  m_Data[m_Length] = 0;
}

template <class C>
inline void CStringTemplate<C>::Append(const C Char) {
  SetSize(m_Length + 2, true);
  m_Data[m_Length] = Char;
  m_Length++;
  m_Data[m_Length] = 0;
}

template <class C>
inline void CStringTemplate<C>::Append(const C * const Buffer, int RightLength) {  
  if (RightLength) {
    SetSize(m_Length + (unsigned int) RightLength + 1, true);
    memcpy(m_Data + m_Length, Buffer, (unsigned int) RightLength);
    m_Length += (unsigned int) RightLength;
    m_Data[m_Length] = 0;
  }
}

template <class C>
inline void CStringTemplate<C>::operator=(const C Char) {  
  SetSize(2, false);
  m_Data[0] = Char;
  m_Data[1] = 0;
  m_Length = 1;
}

template <class C>
inline void CStringTemplate<C>::operator=(const C * const Buffer) {
  CopyBuffer(Buffer);
}

template <class C>
inline void CStringTemplate<C>::operator=(const CStringTemplate<C>& RefString) {  
  CopyBuffer(RefString.m_Data, RefString.m_Length);
}

template <class C>
inline void CStringTemplate<C>::MoveFrom(CStringTemplate<C>& RefString) {
  // buffer on the stack, copy it
  if (! RefString.m_RealAllocData) {
      operator=(RefString);
      RefString.Empty();
  } else {
      
      // delete current buffer
      if (m_RealAllocData) {
          delete[] m_RealAllocData;
      }
      
      // move the buffer
      m_RealAllocData = RefString.m_RealAllocData;
      m_Data = m_RealAllocData;
      m_Size = RefString.m_Size;
      m_Length = RefString.m_Length;
      
      // empty the ref string
      RefString.m_RealAllocData = NULL;      
      RefString.m_Size = CSTRING_STACK_SIZE;
      RefString.m_Data = RefString.m_StackData;
      RefString.m_Data[0] = 0;
      RefString.m_Length = 0;
    
  }
}
    
template <class C>
inline void CStringTemplate<C>::SetLength(void) {
    m_Length = 0;
    for (register int i=0;i<(int)m_Size;i++) {
        if (m_Data[i] == 0) {
            m_Length = i;
            break;
        }
    }
}    
    
template <class C>
inline void CStringTemplate<C>::SetLength(unsigned int Length, C FillChar, bool bFill) {
  SetSize(Length + 1, true);
  if (Length != m_Length) {
    if ((Length > m_Length) && (bFill)) 
      memset(m_Data + m_Length, FillChar, Length - m_Length);
    m_Data[Length] = 0;
    m_Length = Length;
  }
}

template <class C>
inline bool CStringTemplate<C>::Same(const C * String, int Len) const {
  if ((int) m_Length != Len) 
    return false;
  else {
    for (register int i=0;i<(int)Len;i++) {
      if (LCase(m_Data[i]) != LCase(String[i])) 
        return false;
    }
  }
  return true;
}

template <class C>
inline bool CStringTemplate<C>::Equal(const C * String, int Len) const {
  if ((int) m_Length != Len) 
    return false;
  
  for (register int i=0;i<(int)Len;i++) {
    if (m_Data[i] != String[i]) 
      return false;
  }

  return true;
}

template <class C>
inline bool CStringTemplate<C>::Different(const C * String, int Len) const {
  if ((int) m_Length != Len) 
    return true;
 
  for (register int i=0;i<(int)Len;i++) {
    if (m_Data[i] != String[i]) 
      return true;
  }
  
  return false;
}

template <class C>
inline bool CStringTemplate<C>::Smaller(const C * String, int Len) const {
  return (Compare(String, Len, 0) < 0);
}

template <class C>
inline bool CStringTemplate<C>::SmallerInsens(const C * String, int Len) const {
  return (CompareInsens(String, Len, 0) < 0);
}

template <class C>
inline bool CStringTemplate<C>::SmallerOrEqual(const C * String, int Len) const {
  return (Compare(String, Len, 0) <= 0);
}

template <class C>
inline int CStringTemplate<C>::Compare(const C * String, int Len, int Start, bool RespectLength) const {  
  int SelfLength = (int) m_Length - Start;
  int MinLen = (SelfLength < Len)?SelfLength:Len;
  register char c1, c2;
  for (register int i=0;i<MinLen;i++) {
    c1 = m_Data[i + Start];
    c2 = String[i];
    if (c1 != c2)
      return c1 - c2;
  }
  if (!RespectLength)
    return 0;
  if (SelfLength == Len)
    return 0;
  else if (SelfLength > Len)
    return 1;
  else return -1;
}

template <class C>
inline int CStringTemplate<C>::CompareInsens(const C * String, int Len, int Start, bool RespectLength) const {
  int SelfLength = (int) m_Length - Start;
  int MinLen = (SelfLength < Len)?SelfLength:Len;
  register char c1, c2;
  for (register int i=0;i<MinLen;i++) {
    c1 = LCase(m_Data[i + Start]);
    c2 = LCase(String[i]);
    if (c1 != c2)
      return c1 - c2;
  }
  if (!RespectLength)
    return 0;
  if (SelfLength == Len)
    return 0;
  else if (SelfLength > Len)
    return 1;
  else return -1;
}

template <class C>
inline bool CStringTemplate<C>::Bigger(const C * String, int Len) const {
  return (Compare(String, Len, 0) > 0);
}

template <class C>
inline bool CStringTemplate<C>::BiggerInsens(const C * String, int Len) const {
  return (CompareInsens(String, Len, 0) > 0);
}

template <class C>
inline bool CStringTemplate<C>::BiggerOrEqual(const C * String, int Len) const {
  return (Compare(String, Len, 0) >= 0);
}

template <class C>
inline bool CStringTemplate<C>::StartsWithSame(const C * String, int Len) const {
  if ((int) m_Length < Len) 
    return false;
  else {
    for (register int i=0;i<(int)Len;i++) {
      if (LCase(m_Data[i]) != LCase(String[i]))
        return false;
    }
  }
  return true;
}

template <class C>
inline bool CStringTemplate<C>::StartsWith(const C * String, int Len) const {
  if ((int) m_Length < Len) 
    return false;
  else {
    for (register int i=0;i<(int)Len;i++) {
      if (m_Data[i] != String[i])
        return false;
    }
  }
  return true;
}

template <class C>
inline bool CStringTemplate<C>::EndsWithSame(const C * String, int Len) const {
  if ((int) m_Length < Len) 
    return false;
  else {
    for (register int i=0;i<Len;i++) {
      if (LCase(m_Data[i + (m_Length - Len)]) != LCase(String[i]))
        return false;
    }
  }
  return true;
}

template <class C>
inline bool CStringTemplate<C>::EndsWith(const C * String, int Len) const {
  if ((int) m_Length < Len) 
    return false;
  else {
    for (register int i=0;i<Len;i++) {
      if (m_Data[i + (m_Length - Len)] != String[i])
        return false;
    }
  }
  return true;
}

/* insertion */
template <class C>
inline void CStringTemplate<C>::Insert(const unsigned int Position, const CStringTemplate<C>& String) {
  if (Position >= m_Length) Append(String);
  else if (&String == this) {
    CStringTemplate<C> Tmp(String);
    Insert(Position, Tmp);
  } else if (String.m_Length) {
    SetSize(m_Length + String.m_Length + 1, true);
    for (register int i=(int)m_Length;i>=(int)Position;i--)
      m_Data[i + String.m_Length] = m_Data[i];
    memcpy(m_Data + Position, String.m_Data, String.m_Length);
    m_Length += String.m_Length;
  }
}

template <class C>
inline void CStringTemplate<C>::Insert(const unsigned int Position, const C Char) {
  if (Position >= m_Length) Append(Char);
  else {
    SetSize(m_Length + 2, true);
    for (register int i=(int)m_Length;i>=(int)Position;i--)
      m_Data[i+1] = m_Data[i];
    m_Data[Position] = Char;
    m_Length++;
  }
}

template <class C>
inline void CStringTemplate<C>::Insert(const unsigned int Position, const C * const Buffer, int BufferLen) {
  if (Position >= m_Length) Append(Buffer, BufferLen);
  else if (Buffer) {
    if (BufferLen < 0) 
        BufferLen = base_strlen(Buffer);
    if (BufferLen) {
      SetSize(m_Length + (unsigned int) BufferLen + 1, true);
      for (register int i=(int)m_Length;i>=(int)Position;i--)
    m_Data[i + (unsigned int) BufferLen] = m_Data[i];
      memcpy(m_Data + Position, Buffer, BufferLen);
      m_Length += (unsigned int) BufferLen;
    }
  }
}

template <class C>
inline void CStringTemplate<C>::Delete(int Start, int Count) {
  if (Start < 0) {
    Count += Start;
    Start = 0;
  }
  if (Start >= (int) m_Length) return;
  if (Count + Start >= (int) m_Length) Count = m_Length - Start;
  if (Count <= 0) return;
  for (register int i=Start;i<((int)m_Length-Count);i++) {
    m_Data[i] = m_Data[i+Count];
  }
  m_Length-=Count;
  m_Data[m_Length] = 0;
}


template <class C>
inline void CStringTemplate<C>::TrimRight(const C LChar, const C RChar) {
  if (m_Length) {
    C * m_DataRight = m_Data + (int) m_Length - 1;
    while ((m_DataRight>=m_Data)&&(m_DataRight[0]>=LChar)&&(m_DataRight[0]<=RChar)) m_DataRight--;
    if (m_DataRight != (m_Data + (int) m_Length - 1))
      Delete(m_DataRight-m_Data+1, (int)m_Length-(m_DataRight-m_Data)+1);
  }
}

template <class C>
inline void CStringTemplate<C>::TrimLeft(const C LChar, const C RChar) {
  if (m_Length) {
    C * m_DataLeft = m_Data;
    while (((m_DataLeft[0]>=LChar)&&(m_DataLeft[0]<=RChar))&&(m_DataLeft < (m_Data + (int) m_Length))) m_DataLeft++;
    if (m_DataLeft != m_Data) Delete(0, m_DataLeft - m_Data);
  }
}

template <class C>
inline void CStringTemplate<C>::Empty(void) {
  if (m_Length) {
    m_Data[0] = 0;
    m_Length = 0;
  }
}
 
template <class C>
inline void CStringTemplate<C>::CopyBuffer(const C * const Buffer) {
  if (Buffer)
    CopyBuffer(Buffer, base_strlen(Buffer));
  else Empty();
}

template <class C>
inline void CStringTemplate<C>::CopyBuffer(const C * const Buffer, int Length) {
  SetSize(Length + 1, false);
  memcpy(m_Data, Buffer, Length);
  m_Data[Length] = 0;
  m_Length = Length;    
}

template <class C>
inline void CStringTemplate<C>::Initialize(void) {
  m_Data = m_StackData;
  m_RealAllocData = NULL;
  m_Length = 0;
  m_Size = CSTRING_STACK_SIZE;
  m_Data[0] = 0;  
}
