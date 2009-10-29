/*
	part of the MV4 C++ Library

	© Vestris Inc., Geneva Switzerland
	http://www.vestris.com, 1998, All Rights Reserved
	__________________________________________________

	This code has been derived from work by Henry Spencer.
	The main changes are
	1. All char variables and functions have been changed to char
	   counterparts
	2. Added GetFindLen() & GetReplaceString() to enable search
	   and replace operations.
	3. And of course, added the C++ Wrapper

	The original copyright notice follows:

	Copyright (c) 1986, 1993, 1995 by University of Toronto.
	Written by Henry Spencer.  Not derived from licensed software.

	Permission is granted to anyone to use this software for any
	purpose on any computer system, and to redistribute it in any way,
	subject to the following restrictions:

	1. The author is not responsible for the consequences of use of
	   this software, no matter how awful, even if they arise
	   from defects in it.

	2. The origin of this software must not be misrepresented, either
	   by explicit claim or by omission.

	3. Altered versions must be plainly marked as such, and must not
	   be misrepresented (by explicit claim or omission) as being
	   the original software.

    4. This notice must not be removed or altered.

*/

#ifndef BASE_REGEXP_HPP
#define BASE_REGEXP_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>

#define REGEXP_NSUBEXP  10

class CRegExp : public CObject {
  property(CString, Error);
public:
  CRegExp(void);
  ~CRegExp(void);
  static bool SearchReplaceFull(
         const CString& String,
         const CString& SearchExpression,
         const CString& ReplaceExpression,
         int& Position,
         int& ReplaceLength,
         CString& ReplaceString,
         CString * pError = NULL);
  static CString SearchReplace(
         const CString& String,
         const CString& SearchExpression,
         const CString& ReplaceExpression,
         CString * pError = NULL);
  static int Search(
         const CString& String,
         const CString& SearchExpression,
         int& FindLength,
         const int Start = 0);
  static bool Match(const CString& String, const CString& Expression);
  static bool Match(const char *, int, const char *);  
public:
  CRegExp * RegComp( const char * re );
  int RegFind(const char * str);
  char * GetReplaceString( const char * sReplaceExp );
  int GetFindLen();
private:
  char * RegNext(char * node);
  void RegInsert(char op, char * opnd);
  int RegTry(char * string);
  int RegMatch(char * prog);
  size_t RegRepeat(char * node);
  char * Reg(int paren, int * flagp);
  char * RegBranch(int * flagp);
  void RegTail(char * p, char * val);
  void RegOpTail(char * p, char * val);
  char * RegPiece(int * flagp);
  char * RegAtom(int * flagp);
  inline char REGEXP_OP(char * p) { return p?(* p):'\0'; };
  inline char * REGEXP_OPERAND(char * p) { return (char* )((short * )(p+1)+1); };
  void regc(char b); /* regc - emit (if appropriate) a byte of code */
  char * RegNode(char op); /* RegNode - emit a node */
private:
  bool bEmitCode;
  bool bCompiled;
  char * sFoundText;
  char * startp[REGEXP_NSUBEXP];
  char * endp[REGEXP_NSUBEXP];
  char regstart;
  char reganch;
  char * regmust;
  int regmlen;
  char * program;
  char * regparse;        /* Input-scan pointer. */
  int regnpar;            /* () count. */
  char * regcode;         /* Code-emit pointer; &regdummy = don't. */
  char regdummy[3];      /* NOTHING, 0 next ptr */
  long regsize;           /* Code size. */
  char * reginput;        /* String-input pointer. */
  char * regbol;          /* Beginning of input, for ^ check. */
};

/* From the XReplace-32 Users Guide


	^	Beginning of the string. The expression "^A" will match an ‘A’ only at the beginning of the string.
	^	The caret (^) immediately following the left-bracket ([) has a different meaning. It is used to exclude the remaining characters within brackets from matching the target string. The expression "[^0-9]" indicates that the target character should not be a digit.
	$	The dollar sign ($) will match the end of the string. The expression "abc$" will match the sub-string "abc" only if it is at the end of the string.

	|	The alternation character (|) allows either expression on its side to match the target string. The expression "a|b" will match ‘a’ as well as ‘b’.
	.	The dot (.) will match any character.
	*	The asterisk (*) indicates that the character to the left of the asterisk in the expression should match 0 or more times.
	+	The plus (+) is similar to asterisk but there should be at least one match of the character to the left of the + sign in the expression.
	?	The question mark (?) matches the character to its left 0 or 1 times.

	()	The parenthesis affects the order of pattern evaluation and also serves as a tagged expression that can be used when replacing the matched sub-string with another expression.
	[ ]	Brackets ([ and ]) enclosing a set of characters indicates that any of the enclosed characters may match the target character.


	The parenthesis, besides affecting the evaluation order of the regular expression, also serves as tagged expression which is something like a temporary memory. This memory can then be used when we want to replace the found expression with a new expression. The replace expression can specify a & character which means that the & represents the sub-string that was found. So, if the sub-string that matched the regular expression is "abcd", then a replace expression of "xyz&xyz" will change it to "xyzabcdxyz". The replace expression can also be expressed as "xyz\0xyz". The "\0" indicates a tagged expression representing the entire sub-string that was matched. Similarly we can have other tagged expression represented by "\1", "\2" etc. Note that although the tagged expression 0 is always defined, the tagged expression 1,2 etc. are only defined if the regular expression used in the search had enough sets of parenthesis. Here are few examples.

	String		Search		Replace		Result

	Mr.             (Mr)(\.)        \1s\2           Mrs.
	abc             (a)b(c)	        &-\1-\2         abc-a-c
	bcd             (a|b)c*d        &-\1            bcd-b
	abcde           (.*)c(.*)       &-\1-\2         abcde-ab-de
	cde             (ab|cd)e        &-\1            cde-cd

  */

#endif
