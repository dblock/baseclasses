/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_MIME_PARSER_HPP
#define BASE_MIME_PARSER_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <String/StringTable.hpp>
#include <HashTable/VectorTable.hpp>

class CMimeParser : public CObject {
public:
	CMimeParser(void);
	virtual ~CMimeParser(void) = 0;
public:	
	static bool ParseLine(const CString& /* Line */, CString& /* Name */, CString& /* Value */, CStringTable& /* Result */);
	static bool ProcessMultipart(CString /* RawData */, const CString& /* Boudary */, CVectorTable<CStringTable>& /* Fields */);
};

#endif
