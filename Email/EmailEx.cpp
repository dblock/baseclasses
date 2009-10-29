/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
  Revision history:
  
  30.08.1999: implemented minimal sendmail, no MX lookup
  23.09.2001: mime messages
  
  Simple email send through the CEmailExchanger:
  
  CMimeMessage Message;
  
  Message.GetMimeHeader().Set("to", "dblock@vestris.com");
  Message.GetMimeHeader().Set("from", "foo@vestris.com");
  Message.GetMimeHeader().Set("content-type", "text/html");
  Message.GetMimeHeader().Set("subject", "Hello World");
  Message.GetAttachments().Add("/tmp/test.txt");
  
  Message.GetMimeBody().SetBody("<html><body><font face=Helvetica>Hello World</font></body></html>");
  
  CEmailExchanger :: Send("mail.vestris.com", Message);    
  
*/

#include <baseclasses.hpp>
#include "EmailEx.hpp"
#include <Internet/EmailAddress.hpp>
#include <String/GStrings.hpp>

CEmailExchanger::CEmailExchanger(void) {
	
}

CEmailExchanger::~CEmailExchanger(void) {
	
}

bool CEmailExchanger::WriteLine(const CSocket & Socket, const CString& String, CVector<CString>& StatusVector, int ExpectedResult) {
	// cout << "Sending " << String << endl;
	Socket.WriteLine(String);
	CString Line;
    // BUGBUG: some servers return line continuations 250-... instead of 250 OK
	if ((ReadLine(Socket, Line) != ExpectedResult)&&(ExpectedResult!=-1)) {
		StatusVector += (Line + ", mail not sent.");
		// cout << "(Failure) Read " << Line << endl;
		return false;
	} else {
		// cout << "(Success) Read " << Line << endl;
		return true;
	}	
}

int CEmailExchanger::ReadLine(const CSocket& Socket, CString& Line) {
	Socket.ReadLine(&Line);
	int Pos = Line.Pos(' ');
	if (Pos != -1) return Line.GetInt(0, Pos);
	else return -1;
}

CString CEmailExchanger::SnmpToString(const int SCode) {
	switch(SCode) {
	case 500: return "Syntax error, command unrecognized.";
	case 501: return "Syntax error in parameters or arguments.";
	case 502: return "Command not implemented.";
	case 503: return "Bad sequence of commands.";
	case 504: return "Command parameter not implemented.";          
	case 211: return "System status, or system help reply.";
	case 214: return "Help message.";          
	case 220: return "<domain> Service ready.";
	case 221: return "<domain> Service closing transmission channel.";
	case 421: return "<domain> Service not available, closing transmission channel.";          
	case 250: return "Requested mail action okay, completed.";
	case 251: return "User not local; will forward to <forward-path>.";
	case 450: return "Requested mail action not taken: mailbox unavailable.";            
	case 550: return "Requested action not taken: mailbox unavailable.";		
	case 451: return "Requested action aborted: error in processing.";
	case 551: return "User not local; please try <forward-path>.";
	case 452: return "Requested action not taken: insufficient system storage.";
	case 552: return "Requested mail action aborted: exceeded storage allocation.";
	case 553: return "Requested action not taken: mailbox name not allowed.";            
	case 354: return "Start mail input; end with <CRLF>.<CRLF>.";
	case 554: return "Transaction failed.";	
	}
	return "Unsupported error.";
}

bool CEmailExchanger::Send(const CString& MailServer, CMimeMessage& Email) {

    CString ToString;
    Email.GetMimeHeader().Get("to", & ToString);
    ToString.Replace(',',';');
    
    CVector<CString> To;
    CString::StrToVector(ToString, ';', & To);
    
	if (! To.GetSize()) {
		Email.SetStatus("No recepients specified.");
		return false;
	}

	CVector<CString> FinalStatus;
	for (register int i=0;i<(int) To.GetSize(); i++) {		
		
        CEmailAddress EmailAddress(To[i]);

		if (!EmailAddress.GetValid()) {
            FinalStatus += ("Invalid email address " + To[i] + ", mail not sent.");
			continue;
		}
        
		/* send the email through the mail exchanger */	
		CSocket ClientSocket(25, MailServer);
            
		if (! ClientSocket.Open()) {
			FinalStatus += ("Unable to open socket with mail server " + MailServer + ", mail not sent.");
			continue;
		}

        if (! ClientSocket.Connect()) {
			FinalStatus += ("Unable to connect to mail server " + MailServer + ", mail not sent.");
			continue;
        }
            
        CString Line;
        ClientSocket.ReadLine(&Line);
            
		if (!Line.GetLength()) {
			FinalStatus += ("Remote server " + MailServer + " sent invalid response, mail not sent.");
			continue;
		}
        
        CString From;
        Email.GetMimeHeader().Get("from", & From);
        
        if (! From.GetLength()) {
			FinalStatus += ("Missing from address.");
			continue;
        }

		if (    
		    WriteLine(ClientSocket, "HELO " + ClientSocket.GetPeerHostName(), FinalStatus, 250) &&
		    WriteLine(ClientSocket, "MAIL FROM:" + From, FinalStatus, 250) &&
		    WriteLine(ClientSocket, "RCPT TO:" + EmailAddress.GetMail(), FinalStatus, 250) &&
		    WriteLine(ClientSocket, "DATA", FinalStatus, 354)
		    ) {
            
            CString Message;
            Email.GetMessage(& Message);            
			ClientSocket.Write(Message);
    
			if (WriteLine(ClientSocket, ".", FinalStatus, 250)) {
				FinalStatus += ("Successfully sent.");
			}
		}
		ClientSocket.WriteLine("QUIT");		
	}
    
	CString Status;

    for (register int j=0;j<(int)FinalStatus.GetSize();j++) {
		if (j) Status += g_strCrLf;
		Status += (To[j] + " - " + FinalStatus[j]);
	}
    
    Email.SetStatus(Status);
    
	return true;
}
