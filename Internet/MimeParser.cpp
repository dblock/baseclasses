/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include <Internet/MimeParser.hpp>
#include <String/GStrings.hpp>

CMimeParser::CMimeParser(void) {

}

CMimeParser::~CMimeParser(void) {

}

bool CMimeParser::ProcessMultipart(CString RawData, const CString& Boundary, CVectorTable<CStringTable>& Fields) {
  //cout << "expected boundary: [" << "--" + Boundary << "]" << endl;	
  CString OneLine; RawData.ExtractLine(&OneLine);
  //cout << "   found boundary: [" << OneLine << "]" << endl;
  CString PrefixedBoundary;
  while (OneLine.GetLength()) {
    //cout << OneLine << endl;
    PrefixedBoundary = g_strDashDash;
    PrefixedBoundary.Append(Boundary);
    if (OneLine == PrefixedBoundary) {
      CStringTable Table;
      CString Name, Value;
      RawData.ExtractLine(&OneLine);
      while (OneLine.GetLength()) {
        if (ParseLine(OneLine, Name, Value, Table)) {
          //Fields.Add(Name, Table);
          // _L_DEBUG(3, cout << "---------------------" << endl);
          // _L_DEBUG(3, cout << "Content disposition: " << Table.GetValue("Content-Disposition") << endl);
          // _L_DEBUG(3, cout << "               Name: " << Table.GetValue("name") << endl);
          // _L_DEBUG(3, cout << "       Content type: " << Table.GetValue("Content-type") << endl);
          // _L_DEBUG(3, cout << "           Filename: " << Table.GetValue("Filename") << endl);
        } else return false;
        RawData.ExtractLine(&OneLine);
      }            
      int NextBoundaryPosition = RawData.Pos(PrefixedBoundary);
      if (NextBoundaryPosition == -1) NextBoundaryPosition = RawData.GetLength();
      Table.Add(g_strValue, CString(RawData.GetBuffer(), NextBoundaryPosition-2));
      Fields.Add(Table.GetValue(g_strName), Table);
      RawData.Delete(0, NextBoundaryPosition);
      // _L_DEBUG(3, cout << "              Value: " << Table.GetValue(g_strValue) << endl);
      RawData.ExtractLine(&OneLine);
    } else return false;
  }
  return true;
}

bool CMimeParser::ParseLine(const CString& CommandLine, CString& Name, CString& Value, CStringTable& Result) {
  int sPos = CommandLine.Pos(':');
  if (sPos > 0) {
    CommandLine.Mid(0, sPos, &Name);
    Name.Trim();    
    CommandLine.Mid(sPos+1, CommandLine.GetLength(), &Value); 
    Value.Trim();

    if (Name.Same("Authorization") || 
	Name.Same("Location")) {
      Result.Add(Name, Value);
      return true;
    }
 
    int CurPos = 0;
    int PrevPos = 0;
    CString ParameterName = Name;
    CString ParameterValue;
    bool Comment = false;
    while (CurPos < (int) Value.GetLength()) {
      switch(Value[CurPos]) {
      case '(':
        if (!Comment) Comment = true;
        break;
      case ')':
        Comment = false;
        break;
      case '=':
        // double ==, Base64 encoded for example
        if ((CurPos + 1) < (int) Value.GetLength()) {
          if (Value[CurPos + 1] == '=') {
            CurPos++;
            break;
          }
        }
        if (!Comment) {
          Value.Mid(PrevPos, CurPos - PrevPos, &ParameterName);
          ParameterName.Trim();
          PrevPos = CurPos+1;
        }
        break;
      case ';':
        if (!Comment) {
          Value.Mid(PrevPos, CurPos - PrevPos, &ParameterValue);
          ParameterValue.Trim();
          PrevPos = CurPos+1;
          ParameterName.Dequote();
          ParameterValue.Dequote();
          Result.Add(ParameterName, ParameterValue);
          /*
            CVector<CString> ParameterValueVector = CString::StrToVector(ParameterValue, ',');
            for (register int i=0;i<(int)ParameterValueVector.GetSize();i++) {
            _L_DEBUG(3, cout << "adding [" << ParameterName << "][" << ParameterValueVector[i].Trim() << "]" << endl);
            Result.Add(ParameterName, ParameterValueVector[i].Trim().Dequote());
            }
          */
        }
        break;
      default:
        break;
      };
      CurPos++;
    }
    if (PrevPos != (int) Value.GetLength()) {
      Value.Mid(PrevPos, Value.GetLength(), &ParameterValue);
      ParameterValue.Trim();
      /*
        CVector<CString> ParameterValueVector = CString::StrToVector(ParameterValue, ',');
        for (register int i=0;i<(int)ParameterValueVector.GetSize();i++) {
        _L_DEBUG(3, cout << "adding [" << ParameterName << "][" << ParameterValueVector[i].Trim() << "]" << endl);
        Result.Add(ParameterName, ParameterValueVector[i].Trim().Dequote());
        }
      */
      ParameterName.Dequote();
      ParameterValue.Dequote();
      Result.Add(ParameterName, ParameterValue);
    }

    return true;
  } else return false;
}
