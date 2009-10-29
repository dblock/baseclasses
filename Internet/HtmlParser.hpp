/*

    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision History:

    27.08.1999: correctly reading URLs with ...?option=value

*/

#ifndef BASE_HTML_PARSER_HPP
#define BASE_HTML_PARSER_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <Internet/HtmlTag.hpp>
#include <List/List.hpp>
#include <File/Progress.hpp>

class CHtmlParser : public CObject {
  property(bool, Verbose);
  property(bool, ParseHtmlContent);
protected:
  unsigned char m_LastToken;
  int m_InIllegal;
  int m_Errors;
  property(CList<CHtmlTag>, HtmlList);
private:
  static void ReadNameValue(int& curPos, const CString& Token, CString& Name, CString& Value);
  static inline void SkipSpaces(int&, const CString&);
  int ReadText(int& curPos, const CString& iStr, CString *);
  static int ReadString(int& curPos, const CString& Token, bool, CString *);
  bool ReadToken(int& curPos, const CString& iStr, int ForwardDepth, CString& HtmlToken);
  bool PrepareToken(const CString& HtmlToken, const CString& iStr, int& curPos);
  void ParseString(int& curPos, const CString& iStr, CProgress * Progress, int ForwardDepth);
  void ParseToken(const CString&);
  void ForwardParse(int curPos, const CString& iStr, int ForwardDepth);
protected:
  virtual void OnNewTag(const CHtmlTag&);
public:
  static void ParseToken(const CString& Token, CHtmlTag& Target);
  static void TranslateQuote(const CString& Token, CString& Target);
  static void TranslateQuotes(CString&);
  static bool TranslateDecimalToken(const CString& Token, CString& Target);
  static void Quote(const CString& String, CString *);
  static void Quote(CString& String);
  inline static void Dequote(CString& String) { String.Trim('\"'); }
  void Parse(const CString&);
  CHtmlParser(void);
  virtual ~CHtmlParser(void);
  inline CHtmlParser(const CHtmlParser&) { assert(0); }
  inline CHtmlParser& operator=(const CHtmlParser&) { assert(0); return * this; }
  static void QuoteQuotes(const CString& String, CString *);
};

inline void CHtmlParser::SkipSpaces(int& curPos, const CString& Token){
  while ((curPos < (int) Token.GetLength())&&(Token[curPos]<=' ')) curPos++;
}

#endif
