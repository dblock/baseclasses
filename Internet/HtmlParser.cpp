/*

    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    29.08.1999: fixed TransateQuotes to read &#[x]DDD; sequences
    29.08.1999: added TranslateDecimalToken to convert
                &#[x]DDD; sequences
    08.05.2000: forward re-parsing of apparent HTML errors
    
*/

#include <baseclasses.hpp>
#include "HtmlParser.hpp"
#include <File/Progress.hpp>
#include <String/GStrings.hpp>

#define _HTML_QUOT(__HtmlSymbol) ('&' + CString(__HtmlSymbol) + ';')

void CHtmlParser::Quote(CString& String)
{
    CString Target(String);    
    Quote(Target, &String);
}

void CHtmlParser::Quote(const CString& String, CString * pResult) {
    
    typedef struct {
        char m_ch;
        const char * m_pString;
    } CQuoteStruct;
    
    static const CQuoteStruct QuoteStruct[] = {        
        { '&', g_strHtml_amp },
        { '>', g_strHtml_gt },
        { '<', g_strHtml_lt },
        { 'à', g_strHtml_agrave },
        { 'á', g_strHtml_aacute },
        { 'â', g_strHtml_acirc },
        { 'ã', g_strHtml_atilde },
        { 'ä', g_strHtml_auml },
        { 'å', g_strHtml_aring },
        { 'æ', g_strHtml_aelig },
        { 'ç', g_strHtml_ccedil },
        { 'è', g_strHtml_egrave },
        { 'é', g_strHtml_eacute },
        { 'ê', g_strHtml_ecirc },
        { 'ë', g_strHtml_euml },
        { 'ì', g_strHtml_igrave },
        { 'í', g_strHtml_iacute },
        { 'î', g_strHtml_icirc },
        { 'ï', g_strHtml_iuml },
        { 'ð', g_strHtml_eth },
        { 'ñ', g_strHtml_ntilde },
        { 'ò', g_strHtml_ograve },
        { 'ó', g_strHtml_oacute },
        { 'ô', g_strHtml_ocirc },
        { 'õ', g_strHtml_otilde },
        { 'ö', g_strHtml_ouml },
        { '÷', g_strHtml_divide },
        { 'ø', g_strHtml_oslash },
        { 'ù', g_strHtml_ugrave },
        { 'ú', g_strHtml_uacute },
        { 'û', g_strHtml_ucirc },
        { 'ü', g_strHtml_uuml },
        { 'ý', g_strHtml_yacute },
        { 'þ', g_strHtml_thorn },
        { 'ÿ', g_strHtml_yuml },
        { 'À', g_strHtml_Agrave },
        { 'Á', g_strHtml_Aacute },
        { 'Â', g_strHtml_Acirc },
        { 'Ã', g_strHtml_Atilde },
        { 'Ä', g_strHtml_Auml },
        { 'Å', g_strHtml_Aring },
        { 'Æ', g_strHtml_AElig },
        { 'Ç', g_strHtml_Ccedil },
        { 'È', g_strHtml_Egrave },
        { 'É', g_strHtml_Eacute },
        { 'Ê', g_strHtml_Ecirc },
        { 'Ë', g_strHtml_Euml },
        { 'Ì', g_strHtml_Igrave },
        { 'Í', g_strHtml_Iacute },
        { 'Î', g_strHtml_Icirc },
        { 'Ï', g_strHtml_Iuml },
        { 'Ð', g_strHtml_ETH },
        { 'Ñ', g_strHtml_Ntilde },
        { 'Ò', g_strHtml_Ograve },
        { 'Ó', g_strHtml_Oacute },
        { 'Ô', g_strHtml_Ocirc },
        { 'Õ', g_strHtml_Otilde },
        { 'Ö', g_strHtml_Ouml },
        { '×', g_strHtml_times },
        { 'Ø', g_strHtml_Oslash },
        { 'Ù', g_strHtml_Ugrave },
        { 'Ú', g_strHtml_Uacute },
        { 'Û', g_strHtml_Ucirc },
        { 'Ü', g_strHtml_Uuml },
        { 'Ý', g_strHtml_Yacute },
        { 'Þ', g_strHtml_THORN },
        { 'ß', g_strHtml_szlig },
        { '¡', g_strHtml_iexcl },
        { '¢', g_strHtml_cent },
        { '£', g_strHtml_pound },
        { '¤', g_strHtml_curren },
        { '¥', g_strHtml_yen },
        { '¦', g_strHtml_brvbar },
        { '§', g_strHtml_sect },
        { '¨', g_strHtml_uml },
        { '©', g_strHtml_copy },
        { 'ª', g_strHtml_ordf },
        { '«', g_strHtml_laquo },
        { '¬', g_strHtml_not },
        { '­', g_strHtml_shy },
        { '®', g_strHtml_reg },
        { '¯', g_strHtml_macr },
        { '°', g_strHtml_deg },
        { '±', g_strHtml_plusmn },
        { '²', g_strHtml_sup2 },
        { '³', g_strHtml_sup3 },
        { '´', g_strHtml_acute },
        { 'µ', g_strHtml_micro },
        { '¶', g_strHtml_para },
        { '·', g_strHtml_middot },
        { '¸', g_strHtml_cedil },
        { '¹', g_strHtml_sup1 },
        { 'º', g_strHtml_ordm },
        { '»', g_strHtml_raquo },
        { '¼', g_strHtml_frac14 },
        { '½', g_strHtml_frac12 },
        { '¾', g_strHtml_frac34 },
        { '¿', g_strHtml_iquest },
        { '\"', g_strHtml_quot }        
    };
    
    pResult->Empty();
    pResult->SetSize(String.GetLength());

    bool bFound = false;
    
    for (int i=0;i < (int) String.GetLength(); i++) {
        bFound = false;
        for (int j = 0; j < (int) (sizeof(QuoteStruct) / sizeof(CQuoteStruct)); j++) {
            if (String[i] == QuoteStruct[j].m_ch) {
                (* pResult) += _HTML_QUOT(QuoteStruct[j].m_pString);
                bFound = true;
                break;
            }
        }
        
        if (! bFound) {
            (* pResult) += String[i];
        }
    }    
}
    
int CHtmlParser::ReadText(int& curPos, const CString& iStr, CString * pResult){
    
    int prevPos = curPos;
    pResult->Empty();
    while ((curPos < (int) iStr.GetLength())&&(iStr[curPos]!='<'))
        curPos++;
    if (curPos < (int) iStr.GetLength()) m_LastToken = iStr[curPos];
    if (prevPos != curPos)
        return iStr.Mid(prevPos, curPos-prevPos, pResult);
    else return 0;
}

void CHtmlParser::ForwardParse(int curPos, const CString& iStr, int ForwardDepth) {   
    
    // try to parse the remaining string once so we catch both cases    
    if (m_Errors >= 3)
      return;
    m_Errors++;
    if (m_Verbose) {
        cout << '#'; cout.flush();       
    }
    int InIllegalSave = m_InIllegal;  
    int ErrorsSave = m_Errors;
    ParseString(curPos, iStr, NULL, ForwardDepth);  
    m_Errors = ErrorsSave;
    m_InIllegal = InIllegalSave;
}

bool CHtmlParser::ReadToken(int& curPos, const CString& iStr, int ForwardDepth, CString& HtmlToken){
    
    if ((curPos + 1) < (int) iStr.GetLength()) {
        if (iStr[curPos+1] <= ' ') {
            HtmlToken.Empty();
            return true;
        }
    }
    
    int prevPos = curPos + 1;
    int InComment = 0;
    bool InQuote = false;
    
    while (curPos < (int) iStr.GetLength()) {        

        if (iStr[curPos] == '<') {
            if (! InQuote) {
                if (curPos + 3 < (int) iStr.GetLength()) {
                    if ((iStr[curPos + 1] == '!') &&
                        (iStr[curPos + 2] == '-') &&
                        (iStr[curPos + 3] == '-')) {
                        
                        // nested comment
                        
                        InComment++;
                        curPos += 3;
                    }
                }
            }
        } else if (iStr[curPos] == '>') {        
            if ((!InQuote) && (!InComment)) {
                
                curPos++;
                break;
                
            } else if (
                InComment && 
                (curPos >= 2) && 
                (iStr[curPos-1] == '-') &&
                (iStr[curPos-2] == '-')) {
                
                InComment--;
                curPos++;
    
                if (InComment == 0)
                    break;
                
            } else if (InQuote) {            
                
                if (ForwardDepth)
                    return false;
                // ForwardParse(curPos+1, iStr, ForwardDepth+1);          
            }
        } else if (iStr[curPos] == '\"') {
            InQuote = !InQuote;
    
            // common quote mistake name="value""
            while (! InQuote && ((curPos+1) < (int) iStr.GetLength()) && iStr[curPos + 1] == '\"')
                curPos++;
            
        } else if (InQuote && iStr[curPos] == '<') {          
            if (ForwardDepth)
                return false;
            // ForwardParse(curPos, iStr, ForwardDepth+1);
        }
        curPos++;
    }
    
    if (curPos < (int) iStr.GetLength())
        if ((iStr[curPos] == '>')&&(m_LastToken == '<'))
            curPos++;
    int lLen = curPos - prevPos - 1;
    if (InComment)
        lLen -= 2;
    if (lLen > 0)
        iStr.Mid(prevPos, lLen, &HtmlToken);	
    
    return true;
}

bool CHtmlParser::TranslateDecimalToken(const CString& Token, CString& Target) {
    
  /*
    
    The syntax "&#D;", where D is a decimal number, refers to the Unicode
    decimal character number D.
    
    The syntax "&#xH;" or "&#XH;", where H is an hexadecimal number, refers
    to the Unicode hexadecimal character number H. Hexadecimal numbers in
    numeric character references are case-insensitive.
    
  */
  if (Token.GetLength()) {
    int Value;
    if ((Token[0] == 'X')||(Token[0]=='x')) {
      CString MidString;
      Token.Mid(1, Token.GetLength(), &MidString);
      if ((MidString.IsHex(&Value))&&(Value < 255)&&(Value >= 32)) {
        Target.Append((unsigned char) Value);
        return true;
      }
    } else {
      if ((Token.IsInt(&Value))&&(Value < 255)&&(Value >= 32)) {
        Target.Append((unsigned char) Value);
        return true;
      }
    }
  }
  Target.Append('&' + Token + ';');
  return false;
}

void CHtmlParser::TranslateQuote(const CString& Token, CString& Target){
    
    if (Token.GetLength()) {

        if (Token[0] == '#') {
            CString MidString;
            Token.Mid(1, Token.GetLength(), &MidString);
            TranslateDecimalToken(MidString, Target);
            return;
        } 
    
        typedef struct {
            const char * m_pString;
            char m_ch;
        } CQuoteStruct;
    
        static const CQuoteStruct QuoteStruct[] = {
            { g_strHtml_amp, '&' },
            { g_strHtml_nbsp, ' ' },
            { g_strHtml_agrave, 'à' },
            { g_strHtml_aacute, 'á' },
            { g_strHtml_acirc, 'â' },
            { g_strHtml_atilde, 'ã' },
            { g_strHtml_auml, 'ä' },
            { g_strHtml_aring, 'å' },
            { g_strHtml_aelig, 'æ' },
            { g_strHtml_ccedil, 'ç' },
            { g_strHtml_egrave, 'è' },
            { g_strHtml_eacute, 'é' },
            { g_strHtml_ecirc, 'ê' },
            { g_strHtml_euml, 'ë' },
            { g_strHtml_igrave, 'ì' },
            { g_strHtml_iacute, 'í' },
            { g_strHtml_icirc, 'î' },
            { g_strHtml_iuml, 'ï' },
            { g_strHtml_eth, 'ð' },
            { g_strHtml_ntilde, 'ñ' },
            { g_strHtml_ograve, 'ò' },
            { g_strHtml_oacute, 'ó' },
            { g_strHtml_ocirc, 'ô' },
            { g_strHtml_otilde, 'õ' },
            { g_strHtml_ouml, 'ö' },
            { g_strHtml_divide, '÷' },
            { g_strHtml_oslash, 'ø' },
            { g_strHtml_ugrave, 'ù' },
            { g_strHtml_uacute, 'ú' },
            { g_strHtml_ucirc, 'û' },
            { g_strHtml_uuml, 'ü' },
            { g_strHtml_yacute, 'ý' },
            { g_strHtml_thorn, 'þ' },
            { g_strHtml_yuml, 'ÿ' },
            { g_strHtml_Agrave, 'À' },
            { g_strHtml_Aacute, 'Á' },
            { g_strHtml_Acirc, 'Â' },
            { g_strHtml_Atilde, 'Ã' },
            { g_strHtml_Auml, 'Ä' },
            { g_strHtml_Aring, 'Å' },
            { g_strHtml_AElig, 'Æ' },
            { g_strHtml_Ccedil, 'Ç' },
            { g_strHtml_Egrave, 'È' },
            { g_strHtml_Eacute, 'É' },
            { g_strHtml_Ecirc, 'Ê' },
            { g_strHtml_Euml, 'Ë' },
            { g_strHtml_Igrave, 'Ì' },
            { g_strHtml_Iacute, 'Í' },
            { g_strHtml_Icirc, 'Î' },
            { g_strHtml_Iuml, 'Ï' },
            { g_strHtml_ETH, 'Ð' },
            { g_strHtml_Ntilde, 'Ñ' },
            { g_strHtml_Ograve, 'Ò' },
            { g_strHtml_Oacute, 'Ó' },
            { g_strHtml_Ocirc, 'Ô' },
            { g_strHtml_Otilde, 'Õ' },
            { g_strHtml_Ouml, 'Ö' },
            { g_strHtml_times, '×' },
            { g_strHtml_Oslash, 'Ø' },
            { g_strHtml_Ugrave, 'Ù' },
            { g_strHtml_Uacute, 'Ú' },
            { g_strHtml_Ucirc, 'Û' },
            { g_strHtml_Uuml, 'Ü' },
            { g_strHtml_Yacute, 'Ý' },
            { g_strHtml_THORN, 'Þ' },
            { g_strHtml_szlig, 'ß' },
            { g_strHtml_iexcl, '¡' },
            { g_strHtml_cent, '¢' },
            { g_strHtml_pound, '£' },
            { g_strHtml_curren, '¤' },
            { g_strHtml_yen, '¥' },
            { g_strHtml_brvbar, '¦' },
            { g_strHtml_sect, '§' },
            { g_strHtml_uml, '¨' },
            { g_strHtml_copy, '©' },
            { g_strHtml_ordf, 'ª' },
            { g_strHtml_laquo, '«' },
            { g_strHtml_not, '¬' },
            { g_strHtml_shy, '­' },
            { g_strHtml_reg, '®' },
            { g_strHtml_macr, '¯' },
            { g_strHtml_deg, '°' },
            { g_strHtml_plusmn, '±' },
            { g_strHtml_sup2, '²' },
            { g_strHtml_sup3, '³' },
            { g_strHtml_acute, '´' },
            { g_strHtml_micro, 'µ' },
            { g_strHtml_para, '¶' },
            { g_strHtml_middot, '·' },
            { g_strHtml_cedil, '¸' },
            { g_strHtml_sup1, '¹' },
            { g_strHtml_ordm, 'º' },
            { g_strHtml_raquo, '»' },
            { g_strHtml_frac14, '¼' },
            { g_strHtml_frac12, '½' },
            { g_strHtml_frac34, '¾' },
            { g_strHtml_iquest, '¿' },
            { g_strHtml_quot, '\"' }
            // { g_strHtml_gt, '>' }
            // { g_strHtml_lt, '<' }
        };
        
        for (int j=0; j < (int) (sizeof(QuoteStruct) / sizeof(CQuoteStruct)); j++) {
            if (Token.Equal(QuoteStruct[j].m_pString)) {
                Target.Append(QuoteStruct[j].m_ch);
                return;
            }
        }
        
        Target.Append('&' + Token + ';');
    }
}

void CHtmlParser::TranslateQuotes(CString& iStr){
    
    int curPos = 0;
    while (curPos < (int) iStr.GetLength()) {
        if (iStr[curPos] == '&'){
            int iPos = curPos+1;
            while ((iPos < (int) iStr.GetLength())&&(isalnum((unsigned char) iStr[iPos]) || (iStr[iPos] == '#'))) 
              iPos++;
            if ((iPos < (int) iStr.GetLength())&&(iStr[iPos] == ';')) {
              CString InQuote; iStr.Mid(curPos+1, iPos-curPos-1, &InQuote);
              CString PQuote; TranslateQuote(InQuote, PQuote);
              iStr.Delete(curPos, iPos-curPos+1);
              iStr.Insert(curPos, PQuote);
              curPos += PQuote.GetLength();
            } else curPos = iPos;
        } else curPos++;
    }
}

void CHtmlParser::ParseToken(const CString& Token, CHtmlTag& Target) {
    
    int curPos = 0;
    int prevPos = 0;
    if (Token.GetLength() && (Token[0] == '/')) { curPos++; }
    while ((curPos < (int) Token.GetLength())&&(Token[curPos] > ' ')) curPos++;
    CString Name; Token.Mid(prevPos, curPos - prevPos, &Name);
    
    Target.SetName(Name);
    while (curPos < (int) Token.GetLength()) {
        SkipSpaces(curPos, Token);
        if (curPos >= (int) Token.GetLength()) break;
        CString IName; CString IValue;
        ReadNameValue(curPos, Token, IName, IValue);
        
        Target.Set(IName, IValue);
    }
}

void CHtmlParser::ParseToken(const CString& Token){
    
    CHtmlTag CurrentTag;
    ParseToken(Token, CurrentTag);
    m_HtmlList.Add(CurrentTag);
    OnNewTag(CurrentTag);
    
}

int CHtmlParser::ReadString(int& curPos, const CString& Token, bool NameInNameValue, CString * pResult){
    
    SkipSpaces(curPos, Token);
    char pToken = 0;
    int prevPos = curPos;
    int Level = 0;
    while (curPos < (int) Token.GetLength()) {
        if ((Token[curPos] == '=')&&(!Level)&&(NameInNameValue)) break;
        else if ((curPos < (int) Token.GetLength())&&(Token[curPos] <= ' ')&&(!Level)) break;
        else if ((Token[curPos] == '\"')||(Token[curPos] == '\'')) {
            if (!Level) {
                Level++;
                pToken = Token[curPos];
            } else if (Token[curPos] == pToken) {
                Level--;
                curPos++;
                break;
            }
        }
        curPos++;
    }
    Token.Mid(prevPos, curPos-prevPos, pResult);
    if (curPos < (int) Token.GetLength())
        while ((curPos < (int) Token.GetLength())&&(Token[curPos] <= ' ')) 
            curPos++;
        pResult->Trim32();
        return pResult->GetLength();
}

void CHtmlParser::ReadNameValue(int& curPos, const CString& Token, CString& Name, CString& Value){
    
    Value.Empty();
    ReadString(curPos, Token, true, &Name);
    Name.Dequote();
    if ((curPos < (int) Token.GetLength())&&(Token[curPos] == '=')) {
        while ((curPos < (int) Token.GetLength())&&(Token[curPos] == '=')) curPos++;
        ReadString(curPos, Token, false, &Value);
        Value.Dequote();
    }
}

void CHtmlParser::ParseString(int& curPos, const CString& iStr, CProgress * Progress, int ForwardDepth) {
    
    while (curPos < (int) iStr.GetLength()) {
        if (Progress)
            Progress->Show(curPos, iStr.GetLength(), m_Verbose);
        CString FreeText;
        ReadText(curPos, iStr, &FreeText);
        if (FreeText.GetLength()) {
            CHtmlTag Tag;
            Tag.SetFree(FreeText);
            m_HtmlList.Add(Tag);
            OnNewTag(Tag);
        }
        CString HtmlToken;
        if (curPos < (int) iStr.GetLength()) {
            if (!ReadToken(curPos, iStr, ForwardDepth, HtmlToken))
                break;
            
            if (HtmlToken.GetLength()) {
                if (!PrepareToken(HtmlToken, iStr, curPos))
                    ParseToken(HtmlToken);
            } else {
                curPos++;
            }
        } else break;
    }
}

void CHtmlParser::Parse(const CString& iStr) {
    
    CProgress Progress(20, m_Verbose);
    m_InIllegal = 0;
    // m_Errors = 0;
    int curPos = 0;
    if (m_ParseHtmlContent) {
        ParseString(curPos, iStr, &Progress, 0);
    } else {
        CHtmlTag Tag;
        Tag.SetFree(iStr);
        m_HtmlList.Add(Tag);
        OnNewTag(Tag);   
    }
    Progress.Finish(m_Verbose);       
}

bool CHtmlParser::PrepareToken(const CString& HtmlToken, const CString& iStr, int& curPos) {
  
  int tPos = 0;
  if (HtmlToken.GetLength() && HtmlToken[0] == '/') tPos++;
  while ((tPos < (int) HtmlToken.GetLength())&&(isalnum((unsigned char) HtmlToken[tPos]))) tPos++;
  CString TName;
  HtmlToken.Mid(0, tPos, &TName);
  TName.UpperCase();
  if (
      (TName.Equal(g_strHtml_SCRIPT))
      || (TName.Equal(g_strHtml_STYLE)) 
      // || (TName.Equal(g_strHtml_OBJECT))
      ) {
    CString TEndName = ("</" + TName + '>');
    int ePos = iStr.SamePos(TEndName, curPos);
    if (ePos != -1) {
      curPos = (ePos + TEndName.GetLength());
      return true;
    } else m_InIllegal++;
  } else if (
             (TName.Equal(g_strHtml_CSCRIPT))
             || (TName.Equal(g_strHtml_CSTYLE))
             // || (TName.Equal(g_strHtml_COBJECT))
             ) {
    if (m_InIllegal > 0) m_InIllegal--;
  }
  return false;
}


CHtmlParser::CHtmlParser(void) {
    m_Verbose = false;
    m_ParseHtmlContent = true;
}

CHtmlParser::~CHtmlParser(void){
    
}

void CHtmlParser::OnNewTag(const CHtmlTag&) {
    
}

void CHtmlParser::QuoteQuotes(const CString& String, CString * pResult) {
    * pResult = String;
    for (register int i=((int)pResult->GetLength())-1;i>=0;i--)
        switch((* pResult)[i]) {
    case '\"': (* pResult)[i] = ';'; (* pResult).Insert(i, "&quot"); break;
    case '>': (* pResult)[i] = ';'; (* pResult).Insert(i, "&gt"); break;
    case '<': (* pResult)[i] = ';'; (* pResult).Insert(i, "&lt"); break;
    case '&': (* pResult)[i] = ';'; (* pResult).Insert(i, "&amp"); break;
    }
}
