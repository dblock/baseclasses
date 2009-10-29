/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-2001 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "PmdfEmail.hpp"
#include <Internet/MimeParser.hpp>

CPmdfEmail::CPmdfEmail(void) {
	ReadEnviron();	
	if (ReadFromStdin()) {
		CString Buffer = m_RawBuffer;
		Parse(Buffer);
	}
}

CPmdfEmail::~CPmdfEmail(void) {
	
}

bool CPmdfEmail::ReadEnviron(void) {
	int i=0; 
    CString MidString1, MidString2;
    while (_environ[i]) {
        CString Variable = _environ[i++];
        int ePos = Variable.Pos('=');
        if (ePos >= 0) {
            Variable.Mid(0, ePos, &MidString1);
            Variable.Mid(ePos+1, Variable.GetLength(), &MidString2);
            m_Environ.Set(MidString1, MidString2);		
        }
	}
	return true;
}

#define SEGMENT_SIZE 128

bool CPmdfEmail::ReadFromStdin(void) {
	m_RawBuffer.Empty();	
	int Size;
	char Segment[SEGMENT_SIZE];	
	do {		
		Size = fread(Segment, sizeof(char), SEGMENT_SIZE, stdin);		
        m_RawBuffer.Append(Segment, Size);
	} while (Size > 0);
	return (m_RawBuffer.GetLength() > 0);
}

bool CPmdfEmail::Parse(CString& Buffer) {
	static CString UUCPfrom("From ");	
	static CString UUCPfromName("UucpFrom");
	CString Name, Value;
	while (Buffer.GetLength()) {
		CString Line;
        Buffer.ExtractLine(&Line);
        Line.Trim32();
		CStringTable Table;		
		if (Line.StartsWithSame(UUCPfrom)) {
			// UUCP Recepient
            CString MidString;
            Line.Mid(UUCPfrom.GetLength(), Line.GetLength(), &MidString);
			Table.Set(UUCPfromName, MidString);
			m_PmdfFields.Add(UUCPfromName, Table);
			continue;
		} else if (Line.GetLength() && Line[0] == ' ') {
            continue;
		} else if (!Line.GetLength()) {
            break;	
        }
		if (CMimeParser::ParseLine(Line, Name, Value, Table)) {
			m_PmdfFields.Add(Name, Table);
        }
	}
	/* parse the data, the actual email message */
	if (m_PmdfFields.FindElement("Content-Type").GetValue("Content-Type").Same("MULTIPART/MIXED")) {
		/* multipart mixed */
		/* UNSUPPORTED */
	} else {
		/* normal plain text */
        GetMimeBody().SetBody(Buffer);
		Buffer.Empty();
	}
	return true;
}

CString CPmdfEmail::GetPmdfFrom(void) const {
    CString From = GetPmdfValue("From");
    int lPos = From.Pos('<');
    int rPos = From.Pos('>');
    if (rPos > lPos) {
        CString MidString;    
        From.Mid(lPos + 1, rPos - lPos - 1, &MidString);
        return MidString;
    } else return From;
}
