/*
  
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________
  
  Copyright (c) 1986, 1993, 1995 by University of Toronto.
  Written by Henry Spencer.
  Not derived from licensed software.
  _____________________________________________________

  updated by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>
#include "RegExp.hpp"

#define REGEXP_END     0  // no   End of program.
#define REGEXP_BOL     1  // no   Match beginning of line.
#define REGEXP_EOL     2  // no   Match end of line.
#define REGEXP_ANY     3  // no   Match any character.
#define REGEXP_ANYOF   4  // str  Match any of these.
#define REGEXP_ANYBUT  5  // str  Match any but one of these.
#define REGEXP_BRANCH  6  // node Match this, or the next..\&.
#define REGEXP_BACK    7  // no   "next" ptr points backward.
#define REGEXP_EXACTLY 8  // str  Match this string.
#define REGEXP_NOTHING 9  // no   Match empty string.
#define REGEXP_STAR    10 // node Match this 0 or more times.
#define REGEXP_PLUS    11 // node Match this 1 or more times.
#define REGEXP_OPEN    20 // no   Sub-RE starts here.
#define _REGEXP_T(x) (char)(x)

// REGEXP_OPEN+1 is number 1, etc.
#define REGEXP_CLOSE   30 // no   Analogous to REGEXP_OPEN.

// Utility definitions.

#define REGEXP_FAIL(m) { regerror(m); return(NULL); }
#define REGEXP_ISREPN(c) ((c) == _REGEXP_T('*') || (c) == _REGEXP_T('+') || (c) == _REGEXP_T('?'))
#define REGEXP_META "^$.[()|?+*\\"

// Flags to be passed up and down.

#define REGEXP_HASWIDTH  01 // Known never to match null string.
#define REGEXP_SIMPLE    02 // Simple enough to be REGEXP_STAR/REGEXP_PLUS operand.
#define REGEXP_SPSTART   04 // Starts with * or +.
#define REGEXP_WORST     0  // Worst case.

char * CRegExp::RegNode(char op) {
  if (!bEmitCode) {
    regsize += 3;
    return regcode;
  }
  
  *regcode++ = op;
  *regcode++ = _REGEXP_T('\0');          /* Null next pointer. */
  *regcode++ = _REGEXP_T('\0');
  
  return regcode-3;
}

void CRegExp::regc(char b) {
  if (bEmitCode)
    *regcode++ = b;
  else
    regsize++;
}

int CRegExp::GetFindLen() {
  if( startp[0] == NULL || endp[0] == NULL )
    return 0;
  return endp[0] - startp[0];
}

CRegExp::CRegExp() {
  bCompiled = false;
  program = NULL;
  sFoundText = NULL;
  
  for( int i = 0; i < REGEXP_NSUBEXP; i++ ) {
    startp[i] = NULL;
    endp[i] = NULL;
  }
}

CRegExp::~CRegExp() {
  if (program)
    delete[] program;
  if (sFoundText)
	delete[] sFoundText;
}

CRegExp * CRegExp::RegComp(const char *exp) {
  char *scan;
  int flags;
  
  if (exp == NULL)
    return NULL;
  
  bCompiled = true;
  
  // First pass: determine size, legality.
  bEmitCode = false;
  regparse = (char *)exp;
  regnpar = 1;
  regsize = 0L;
  regdummy[0] = REGEXP_NOTHING;
  regdummy[1] = regdummy[2] = 0;
  regcode = regdummy;

  if (!Reg(0, &flags))
    return NULL;
  
  // Allocate space.
  if (program)
    delete[] program;
  program = new char[regsize];
  memset( program, 0, regsize * sizeof(char) );
  
  if (!program)
    return NULL;

  // Second pass: emit code.
  bEmitCode = true;
  regparse = (char *)exp;
  regnpar = 1;
  regcode = program;
  if (!Reg(0, &flags))
    return NULL;
  
  // Dig out information for optimizations.
  regstart = _REGEXP_T('\0');            // Worst-case defaults.
  reganch = 0;
  regmust = NULL;
  regmlen = 0;
  scan = program;         // First REGEXP_BRANCH.
  if (REGEXP_OP(RegNext(scan)) == REGEXP_END) {
    // Only one top-level choice.
    scan = REGEXP_OPERAND(scan);
    
    // Starting-point info.
    if (REGEXP_OP(scan) == REGEXP_EXACTLY)
      regstart = *REGEXP_OPERAND(scan);
    else if (REGEXP_OP(scan) == REGEXP_BOL)
      reganch = 1;
    
    // If there's something expensive in the r.e., find the
    // longest literal string that must appear and make it the
    // regmust.  Resolve ties in favor of later strings, since
    // the regstart check works with the beginning of the r.e.
    // and avoiding duplication strengthens checking.  Not a
    // strong reason, but sufficient in the absence of others.
    
    if (flags&REGEXP_SPSTART)
      {
        char *longest = NULL;
        size_t len = 0;
        
        for (; scan != NULL; scan = RegNext(scan))
          if (REGEXP_OP(scan) == REGEXP_EXACTLY && base_strlen(REGEXP_OPERAND(scan)) >= (unsigned int) len)
            {
              longest = REGEXP_OPERAND(scan);
              len = base_strlen(REGEXP_OPERAND(scan));
            }
        regmust = longest;
        regmlen = (int)len;
      }
  }  
  return this;
}

// reg - regular expression, i.e. main body or parenthesized thing
//
// Caller must absorb opening parenthesis.
//
// Combining parenthesis handling with the base level of regular expression
// is a trifle forced, but the need to tie the tails of the branches to what
// follows makes it hard to avoid.

char *CRegExp::Reg(int paren, int *flagp) {
  char *ret;
  char *br;
  char *brloop;
  char *ender;
  int parno;
  int flags;
  
  * flagp = REGEXP_HASWIDTH;      // Tentatively.
  
  if (paren)
	{
      // Make an REGEXP_OPEN node.
      if (regnpar >= REGEXP_NSUBEXP)
		{
          //TRACE1("Too many (). REGEXP_NSUBEXP is set to %d\n", REGEXP_NSUBEXP );
          m_Error = "Too many ()";
          return NULL;
		}
      parno = regnpar;
      regnpar++;
      ret = RegNode(REGEXP_OPEN+parno);
	}
  
  // Pick up the branches, linking them together.
  br = RegBranch(&flags);
  if (br == NULL)
    return(NULL);
  if (paren)
    RegTail(ret, br);       // REGEXP_OPEN -> first.
  else
    ret = br;
  *flagp &= ~(~flags&REGEXP_HASWIDTH);   // Clear bit if bit 0.
  *flagp |= flags&REGEXP_SPSTART;
  while (*regparse == _REGEXP_T('|')) {
    regparse++;
    br = RegBranch(&flags);
    if (br == NULL)
      return(NULL);
    RegTail(ret, br);       // REGEXP_BRANCH -> REGEXP_BRANCH.
    *flagp &= ~(~flags&REGEXP_HASWIDTH);
    *flagp |= flags&REGEXP_SPSTART;
  }
  
  // Make a closing node, and hook it on the end.
  ender = RegNode((paren) ? REGEXP_CLOSE+parno : REGEXP_END);
  RegTail(ret, ender);
  
  // Hook the tails of the branches to the closing node.
  for (br = brloop = ret; (br != NULL) && (br >= brloop); br = RegNext(brloop = br))
    RegOpTail(br, ender);  
  
  // Check for proper termination.
  if (paren && *regparse++ != _REGEXP_T(')')) {
    //TRACE0("unterminated ()\n");
    m_Error = "unterminated ()";
    return NULL;
  } else if (!paren && *regparse != _REGEXP_T('\0')) {
    if (*regparse == _REGEXP_T(')')) {
      //TRACE0("unmatched ()\n");
      m_Error = "unmatched ()";
      return NULL;
    } else {
      //TRACE0("internal error: junk on end\n");
      m_Error = "internal error: junk on end";
      return NULL;
    }
    // NOTREACHED
  }  
  return(ret);
}

//
// RegBranch - one alternative of an | operator
//
// Implements the concatenation operator.
char *CRegExp::RegBranch(int *flagp) {
  char *ret;
  char *chain;
  char *latest;
  int flags;
  int c;
  
  *flagp = REGEXP_WORST;                         // Tentatively.
  
  ret = RegNode(REGEXP_BRANCH);
  chain = NULL;
  while ((c = *regparse) != _REGEXP_T('\0') && c != _REGEXP_T('|') && c != _REGEXP_T(')')) {
    latest = RegPiece(&flags);
    if (latest == NULL)
      return(NULL);
    *flagp |= flags&REGEXP_HASWIDTH;
    if (chain == NULL)              // First piece.
      *flagp |= flags&REGEXP_SPSTART;
    else
      RegTail(chain, latest);
    chain = latest;
  }
  if (chain == NULL)                      // Loop ran zero times.
    (void) RegNode(REGEXP_NOTHING); 
  return ret;
}

//
// RegPiece - something followed by possible [*+?]
//
// Note that the branching code sequences used for ? and the general cases
// of * and + are somewhat optimized:  they use the same REGEXP_NOTHING node as
// both the endmarker for their branch list and the body of the last branch.
// It might seem that this node could be dispensed with entirely, but the
// endmarker role is not redundant.

char *CRegExp::RegPiece(int *flagp) {
  char *ret;
  char op;
  char *next;
  int flags;

  ret = RegAtom(&flags);
  if (ret == NULL)
    return(NULL);

  op = *regparse;
  if (!REGEXP_ISREPN(op)) {
    *flagp = flags;
    return(ret);
  }

  if (!(flags&REGEXP_HASWIDTH) && op != _REGEXP_T('?'))
	{
      //TRACE0("*+ operand could be empty\n");
      m_Error = "*+ operand could be empty";
      return NULL;
	}

  switch (op) {
  case _REGEXP_T('*'):   *flagp = REGEXP_WORST|REGEXP_SPSTART;                 break;
  case _REGEXP_T('+'):   *flagp = REGEXP_WORST|REGEXP_SPSTART|REGEXP_HASWIDTH;        break;
  case _REGEXP_T('?'):   *flagp = REGEXP_WORST;                         break;
  }

  if (op == _REGEXP_T('*') && (flags&REGEXP_SIMPLE))
    RegInsert(REGEXP_STAR, ret);
  else if (op == _REGEXP_T('*')) {
    // Emit x* as (x&|), where & means "self".
    RegInsert(REGEXP_BRANCH, ret);         // Either x
    RegOpTail(ret, RegNode(REGEXP_BACK));  // and loop
    RegOpTail(ret, ret);            // back
    RegTail(ret, RegNode(REGEXP_BRANCH));  // or
    RegTail(ret, RegNode(REGEXP_NOTHING)); // null.
  } else if (op == _REGEXP_T('+') && (flags&REGEXP_SIMPLE))
    RegInsert(REGEXP_PLUS, ret);
  else if (op == _REGEXP_T('+')) {
    // Emit x+ as x(&|), where & means "self".
    next = RegNode(REGEXP_BRANCH);         // Either
    RegTail(ret, next);
    RegTail(RegNode(REGEXP_BACK), ret);    // loop back
    RegTail(next, RegNode(REGEXP_BRANCH)); // or
    RegTail(ret, RegNode(REGEXP_NOTHING)); // null.
  } else if (op == _REGEXP_T('?')) {
    // Emit x? as (x|)
    RegInsert(REGEXP_BRANCH, ret);         // Either x
    RegTail(ret, RegNode(REGEXP_BRANCH));  // or
    next = RegNode(REGEXP_NOTHING);                // null.
    RegTail(ret, next);
    RegOpTail(ret, next);
  }
  regparse++;
  if (REGEXP_ISREPN(*regparse))
	{
      //TRACE0("nested *?+\n");
      m_Error = "nested *?+";
      return NULL;
	}

  return(ret);
}

//
// RegAtom - the lowest level
//
// Optimization:  gobbles an entire sequence of ordinary characters so that
// it can turn them into a single node, which is smaller to store and
// faster to run.  Backslashed characters are exceptions, each becoming a
// separate node; the code is simpler that way and it's not worth fixing.

char *CRegExp::RegAtom(int *flagp) {
  char *ret;
  int flags;

  *flagp = REGEXP_WORST;         // Tentatively.

  switch (*regparse++) {
  case _REGEXP_T('^'):
    ret = RegNode(REGEXP_BOL);
    break;
  case _REGEXP_T('$'):
    ret = RegNode(REGEXP_EOL);
    break;
  case _REGEXP_T('.'):
    ret = RegNode(REGEXP_ANY);
    *flagp |= REGEXP_HASWIDTH|REGEXP_SIMPLE;
    break;
  case _REGEXP_T('['): {
    int range;
    int rangeend;
    int c;

    if (*regparse == _REGEXP_T('^')) {     // Complement of range.
      ret = RegNode(REGEXP_ANYBUT);
      regparse++;
    } else
      ret = RegNode(REGEXP_ANYOF);
    if ((c = *regparse) == _REGEXP_T(']') || c == _REGEXP_T('-')) {
      regc(c);
      regparse++;
    }
    while ((c = *regparse++) != _REGEXP_T('\0') && c != _REGEXP_T(']')) {
      if (c != _REGEXP_T('-'))
        regc(c);
      else if ((c = *regparse) == _REGEXP_T(']') || c == _REGEXP_T('\0'))
        regc(_REGEXP_T('-'));
      else
        {
          range = (unsigned) (char)*(regparse-2);
          rangeend = (unsigned) (char)c;
          if (range > rangeend)
            {
              //TRACE0("invalid [] range\n");
              m_Error = "invalid [] range";
              return NULL;
            }
          for (range++; range <= rangeend; range++)
            regc(range);
          regparse++;
        }
    }
    regc(_REGEXP_T('\0'));
    if (c != _REGEXP_T(']'))
      {
        //TRACE0("unmatched []\n");
        m_Error = "unmatched []";
        return NULL;
      }
    *flagp |= REGEXP_HASWIDTH|REGEXP_SIMPLE;
    break;
  }
  case _REGEXP_T('('):
    ret = Reg(1, &flags);
    if (ret == NULL)
      return(NULL);
    *flagp |= flags&(REGEXP_HASWIDTH|REGEXP_SPSTART);
    break;
  case _REGEXP_T('\0'):
  case _REGEXP_T('|'):
  case _REGEXP_T(')'):
    // supposed to be caught earlier
    //TRACE0("internal error: \\0|) unexpected\n");
    m_Error = "internal error: \\0|) unexpected";
    return NULL;
    break;
  case _REGEXP_T('?'):
  case _REGEXP_T('+'):
  case _REGEXP_T('*'):
    //TRACE0("?+* follows nothing\n");
    m_Error = "?+* follows nothing";
    return NULL;
    break;
  case _REGEXP_T('\\'):
    if (*regparse == _REGEXP_T('\0'))
      {
        //TRACE0("trailing \\\n");
        m_Error = "trailing \\\n";
        return NULL;
      }
    ret = RegNode(REGEXP_EXACTLY);
    regc(*regparse++);
    regc(_REGEXP_T('\0'));
    *flagp |= REGEXP_HASWIDTH|REGEXP_SIMPLE;
    break;
  default: {
      size_t len;
      char ender;

      regparse--;
      len = base_strcspn(regparse, REGEXP_META);
      if (len == 0)
		{
          //TRACE0("internal error: strcspn 0\n");
          m_Error = "internal error: strcspn 0";
          return NULL;
		}
      ender = *(regparse+len);
      if (len > 1 && REGEXP_ISREPN(ender))
        len--;          // Back off clear of ?+* operand.
      *flagp |= REGEXP_HASWIDTH;
      if (len == 1)
        *flagp |= REGEXP_SIMPLE;
      ret = RegNode(REGEXP_EXACTLY);
      for (; len > 0; len--)
        regc(*regparse++);
      regc(_REGEXP_T('\0'));
      break;
    }
  }

  return(ret);
}



// RegInsert - insert an operator in front of already-emitted operand
//
// Means relocating the operand.

void CRegExp::RegInsert(char op, char *opnd) {
  char *place;

  if (!bEmitCode) {
    regsize += 3;
    return;
  }

  (void) memmove(opnd+3, opnd, (size_t)((regcode - opnd)*sizeof(char)));
  regcode += 3;

  place = opnd;           // Op node, where operand used to be.
  *place++ = op;
  *place++ = _REGEXP_T('\0');
  *place++ = _REGEXP_T('\0');
}

//
// RegTail - set the next-pointer at the end of a node chain

void CRegExp::RegTail(char *p, char *val) {
  char *scan;
  char *temp;
  //      int offset;

  if (!bEmitCode)
    return;

  // Find last node.
  for (scan = p; (temp = RegNext(scan)) != NULL; scan = temp)
    continue;

  * (scan + 1) =(REGEXP_OP(scan) == REGEXP_BACK) ? scan - val : val - scan;
}

// RegOpTail - RegTail on operand of first argument; nop if operandless

void CRegExp::RegOpTail(char *p, char *val) {
  // "Operandless" and "op != REGEXP_BRANCH" are synonymous in practice.
  if (!bEmitCode || REGEXP_OP(p) != REGEXP_BRANCH)
    return;
  RegTail(REGEXP_OPERAND(p), val);
}


// RegFind      - match a regexp against a string
// Returns      - Returns position of regexp or -1
//                        if regular expression not found
// Note         - The regular expression should have been
//                        previously compiled using RegComp
int CRegExp::RegFind(const char *str) {
  char *string = (char *)str;   // avert const poisoning
  char *s;

  // Delete any previously stored found string
  if (sFoundText)
    delete[] sFoundText;
  sFoundText = NULL;

  // Be paranoid.
  if(string == NULL)
	{
      //TRACE0("NULL argument to regexec\n");
      m_Error = "NULL argument to regexec";
      return(-1);
	}

  // Check validity of regex
  if (!bCompiled)
	{
      //TRACE0("No regular expression provided yet.\n");
      m_Error = "No regular expression provided yet.";
      return(-1);
	}

  // If there is a "must appear" string, look for it.
  if (regmust != NULL && base_strstr(string, regmust) == NULL)
    return(-1);

  // Mark beginning of line for ^
  regbol = string;

  // Simplest case:  anchored match need be tried only once.
  if (reganch)
	{
      if( RegTry(string) )
		{
          // Save the found substring in case we need it
          sFoundText = new char[GetFindLen()+1];
          sFoundText[GetFindLen()] = _REGEXP_T('\0');
          base_strncpy(sFoundText, string, GetFindLen() );

          return 0;
		}
      //String not found
      return -1;
	}

  // Messy cases:  unanchored match.
  if (regstart != _REGEXP_T('\0'))
	{
      // We know what char it must start with.
      for (s = string; s != NULL; s = base_strchr(s+1, regstart))
        if (RegTry(s))
          {
            int nPos = s-str;

            // Save the found substring in case we need it later
            sFoundText = new char[GetFindLen()+1];
            sFoundText[GetFindLen()] = _REGEXP_T('\0');
            base_strncpy(sFoundText, s, GetFindLen() );

            return nPos;
          }
      return -1;
	}
  else
	{
      // We don't -- general case
      for (s = string; !RegTry(s); s++)
        if (*s == _REGEXP_T('\0'))
          return(-1);

      int nPos = s-str;

      // Save the found substring in case we need it later
      sFoundText = new char[GetFindLen()+1];
      sFoundText[GetFindLen()] = _REGEXP_T('\0');
      base_strncpy(sFoundText, s, GetFindLen() );

      return nPos;
	}
  // NOTREACHED
}


// RegTry - try match at specific point

int     CRegExp::RegTry(char *string) {
  int i;
  char **stp;
  char **enp;

  reginput = string;

  stp = startp;
  enp = endp;
  for (i = REGEXP_NSUBEXP; i > 0; i--)
	{
      *stp++ = NULL;
      *enp++ = NULL;
	}
  if (RegMatch(program))
	{
      startp[0] = string;
      endp[0] = reginput;
      return(1);
	}
  else
    return(0);
}

// RegMatch - main matching routine
//
// Conceptually the strategy is simple:  check to see whether the current
// node matches, call self recursively to see whether the rest matches,
// and then act acbaseingly.  In practice we make some effort to avoid
// recursion, in particular by going through "ordinary" nodes (that don't
// need to know whether the rest of the match failed) by a loop instead of
// by recursion.

int     CRegExp::RegMatch(char *prog) {
  char *scan;    // Current node.
  char *next;            // Next node.

  for (scan = prog; scan != NULL; scan = next) {
    next = RegNext(scan);

    switch (REGEXP_OP(scan)) {
    case REGEXP_BOL:
      if (reginput != regbol)
        return(0);
      break;
    case REGEXP_EOL:
      if (*reginput != _REGEXP_T('\0'))
        return(0);
      break;
    case REGEXP_ANY:
      if (*reginput == _REGEXP_T('\0'))
        return(0);
      reginput++;
      break;
    case REGEXP_EXACTLY: {
      size_t len;
      char *const opnd = REGEXP_OPERAND(scan);

      // Inline the first character, for speed.
      if (*opnd != *reginput)
        return(0);
      len = base_strlen(opnd);
      if (len > 1 && base_strncmp(opnd, reginput, len) != 0)
        return(0);
      reginput += len;
      break;
    }
    case REGEXP_ANYOF:
      if (*reginput == _REGEXP_T('\0') ||
          base_strchr(REGEXP_OPERAND(scan), *reginput) == NULL)
        return(0);
      reginput++;
      break;
    case REGEXP_ANYBUT:
      if (*reginput == _REGEXP_T('\0') ||
          base_strchr(REGEXP_OPERAND(scan), *reginput) != NULL)
        return(0);
      reginput++;
      break;
    case REGEXP_NOTHING:
      break;
    case REGEXP_BACK:
      break;
    case REGEXP_OPEN+1: case REGEXP_OPEN+2: case REGEXP_OPEN+3:
    case REGEXP_OPEN+4: case REGEXP_OPEN+5: case REGEXP_OPEN+6:
    case REGEXP_OPEN+7: case REGEXP_OPEN+8: case REGEXP_OPEN+9: {
      const int no = REGEXP_OP(scan) - REGEXP_OPEN;
      char *const input = reginput;

      if (RegMatch(next)) {
        // Don't set startp if some later
        // invocation of the same parentheses
        // already has.

        if (startp[no] == NULL)
          startp[no] = input;
        return(1);
      } else
        return(0);
      break;
    }
    case REGEXP_CLOSE+1: case REGEXP_CLOSE+2: case REGEXP_CLOSE+3:
    case REGEXP_CLOSE+4: case REGEXP_CLOSE+5: case REGEXP_CLOSE+6:
    case REGEXP_CLOSE+7: case REGEXP_CLOSE+8: case REGEXP_CLOSE+9: {
      const int no = REGEXP_OP(scan) - REGEXP_CLOSE;
      char *const input = reginput;

      if (RegMatch(next)) {
        // Don't set endp if some later
        // invocation of the same parentheses
        // already has.

        if (endp[no] == NULL)
          endp[no] = input;
        return(1);
      } else
        return(0);
      break;
    }
    case REGEXP_BRANCH: {
      char *const save = reginput;

      if (REGEXP_OP(next) != REGEXP_BRANCH)         // No choice.
        next = REGEXP_OPERAND(scan);   // Avoid recursion.
      else {
        while (REGEXP_OP(scan) == REGEXP_BRANCH) {
          if (RegMatch(REGEXP_OPERAND(scan)))
            return(1);
          reginput = save;
          scan = RegNext(scan);
        }
        return(0);
        // NOTREACHED
      }
      break;
    }
    case REGEXP_STAR:
    case REGEXP_PLUS: {
      const char nextch =
        (REGEXP_OP(next) == REGEXP_EXACTLY) ? *REGEXP_OPERAND(next) : _REGEXP_T('\0');
      size_t no;
      char *const save = reginput;
      const size_t min = (REGEXP_OP(scan) == REGEXP_STAR) ? 0 : 1;

      for (no = RegRepeat(REGEXP_OPERAND(scan)) + 1; no > min; no--) {
        reginput = save + no - 1;
        // If it could work, try it.
        if (nextch == _REGEXP_T('\0') || *reginput == nextch)
          if (RegMatch(next))
            return(1);
      }
      return(0);
      break;
    }
    case REGEXP_END:
      return(1);      // Success!
      break;
    default:
      //TRACE0("regexp corruption\n");
      m_Error = "regexp corruption";
      return(0);
      break;
    }
  }

  // We get here only if there's trouble -- normally "case REGEXP_END" is
  // the terminating point.

  //TRACE0("corrupted pointers\n");
  m_Error = "corrupted pointers";
  return(0);
}


// RegRepeat - report how many times something simple would match

size_t CRegExp::RegRepeat(char *node) {
  size_t count;
  char *scan;
  char ch;

  switch (REGEXP_OP(node))
	{
	case REGEXP_ANY:
      return(base_strlen(reginput));
      break;
	case REGEXP_EXACTLY:
      ch = *REGEXP_OPERAND(node);
      count = 0;
      for (scan = reginput; *scan == ch; scan++)
        count++;
      return(count);
      break;
	case REGEXP_ANYOF:
      return(base_strspn(reginput, REGEXP_OPERAND(node)));
      break;
	case REGEXP_ANYBUT:
      return(base_strcspn(reginput, REGEXP_OPERAND(node)));
      break;
	default:                // Oh dear.  Called inappropriately.
      //TRACE0("internal error: bad call of RegRepeat\n");
      m_Error = "internal error: bad call of RegRepeat";
      return(0);      // Best compromise.
      break;
	}
  // NOTREACHED
}

// RegNext - dig the "next" pointer out of a node

char *CRegExp::RegNext(char *p) {
  if (!p)
    return NULL;
  
  const short &offset = (short) (* (p + 1));  
  
  if (!offset)
    return NULL;
  
  return((REGEXP_OP(p) == REGEXP_BACK) ? p-offset : p+offset);
}

// GetReplaceString     - Converts a replace expression to a string
// Returns              - Pointer to newly allocated string
//                        Caller is responsible for deleting it
char* CRegExp::GetReplaceString( const char* sReplaceExp ) {
  char *src = (char *)sReplaceExp;
  char *buf;
  char c;
  int no;
  size_t len;

  if( sReplaceExp == NULL || sFoundText == NULL )
    return NULL;

  // First compute the length of the string
  int replacelen = 0;
  while ((c = *src++) != _REGEXP_T('\0'))
	{
      if (c == _REGEXP_T('&'))
        no = 0;
      else if (c == _REGEXP_T('\\') && isdigit(*src))
        no = *src++ - _REGEXP_T('0');
      else
        no = -1;

      if (no < 0)
		{
          // Ordinary character.
          if (c == _REGEXP_T('\\') && (*src == _REGEXP_T('\\') || *src == _REGEXP_T('&')))
            c = *src++;
          replacelen++;
		}
      else if (startp[no] != NULL && endp[no] != NULL &&
               endp[no] > startp[no])
		{
          // Get tagged expression
          len = endp[no] - startp[no];
          replacelen += len;
		}
	}

  // Now allocate buf
  buf = new char[replacelen+1];
  if( buf == NULL )
    return NULL;

  char* sReplaceStr = buf;

  // Add null termination
  buf[replacelen] = _REGEXP_T('\0');

  // Now we can create the string
  src = (char *)sReplaceExp;
  while ((c = *src++) != _REGEXP_T('\0'))
	{
      if (c == _REGEXP_T('&'))
        no = 0;
      else if (c == _REGEXP_T('\\') && isdigit(*src))
        no = *src++ - _REGEXP_T('0');
      else
        no = -1;

      if (no < 0)
		{
          // Ordinary character.
          if (c == _REGEXP_T('\\') && (*src == _REGEXP_T('\\') || *src == _REGEXP_T('&')))
            c = *src++;
          *buf++ = c;
		}
      else if (startp[no] != NULL && endp[no] != NULL &&
               endp[no] > startp[no])
		{
          // Get tagged expression
          len = endp[no] - startp[no];
          int tagpos = startp[no] - startp[0];

          base_strncpy(buf, sFoundText + tagpos, len);
          buf += len;
		}
	}

  return sReplaceStr;
}

/* added by Daniel Doubrovkine, the C++ wrapper */

CString CRegExp::SearchReplace(const CString& String,
                               const CString& SearchExpression,
                               const CString& ReplaceExpression,
                               CString * pError
                               ) {
  CString Result = String;
  int Position;
  int ReplaceLength;
  SearchReplaceFull(String,
                    SearchExpression,
                    ReplaceExpression,
                    Position,
                    ReplaceLength,
                    Result,
                    pError);
  return Result;
}

bool CRegExp::Match(const CString& String, const CString& Expression) {  
  return Match(String.GetBuffer(), (int) String.GetLength(), Expression.GetBuffer());
}
 
bool CRegExp::Match(const char * String, int StringLen, const char * Expression) {
  CRegExp LocalRegExp;
  LocalRegExp.RegComp(Expression);
  int Position = LocalRegExp.RegFind(String);
  if ((Position == 0) && (LocalRegExp.GetFindLen() == StringLen))
    return true;
  return false;  
}

bool CRegExp::SearchReplaceFull(const CString& String,
                                const CString& SearchExpression,
                                const CString& ReplaceExpression,
                                int& Position,
                                int& ReplaceLength,
                                CString& ReplaceString,
                                CString * pError) {
  bool bResult = false;
  CRegExp LocalRegExp;
  LocalRegExp.RegComp( (const char *) SearchExpression.GetBuffer() );
  if ( (Position = LocalRegExp.RegFind((const char *) String.GetBuffer()) != -1)) {
    char * pReplaceStr = LocalRegExp.GetReplaceString((const char *) ReplaceExpression.GetBuffer());
    ReplaceString = pReplaceStr;
    if (pReplaceStr)
      delete[] pReplaceStr;
    ReplaceLength = LocalRegExp.GetFindLen();
    bResult = true;
  } 
  if (pError) 
    * pError = LocalRegExp.GetError();
  return bResult;
}

int CRegExp::Search(const CString& String,
                    const CString& SearchExpression,
                    int& FindLength,
                    const int Start) {
  
  if (Start >= (int) String.GetLength()) 
    return -1;
  CRegExp LocalRegExp;	
  LocalRegExp.RegComp( (const char *) SearchExpression.GetBuffer());
  int Position;
  if ( (Position = LocalRegExp.RegFind((const char *) String.GetBuffer() + Start) != -1)) {
    FindLength = LocalRegExp.GetFindLen();
    return Position + Start;
  } else {
    return -1;
  }
}
