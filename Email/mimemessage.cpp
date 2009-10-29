/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-2001 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#include <baseclasses.hpp>
#include <Encryption/Encryption.hpp>
#include "mimemessage.hpp"
    
CMimeMessage :: CMimeMessage(void) {
    m_MimeHeader.Add("mime-version", "1.0");
    m_MimeHeader.Add("x-priority", "3");
    m_MimeHeader.Add("x-mailer", "Vestris BC++");
    m_MimeHeader.Add("x-mimeole", "Sent with Vestris BC++ (http://www.vestris.com)");
}

CMimeMessage :: ~CMimeMessage(void) {
    
}

bool CMimeMessage :: GetContentHeader(const CString& ssContentType, const CString& ssFilePath, CString * pssResult) const {
    
    long nPos;
    CString ssResult;
    
    if (! pssResult)
        return false;
    
    if (ssFilePath.GetLength()) {

        CString ssShortName;

        nPos = ssFilePath.InvPos('\\');
        if (nPos != -1) {
            ssFilePath.Mid(nPos + 1, ssFilePath.GetLength(), & ssShortName);
        } else {
            ssShortName = ssFilePath;
        }
        
        if (ssContentType.GetLength())
            ssResult += ssContentType;    
        else ssResult += "text/plain";
        
        ssResult += ";";
        ssResult += "\r\n\tname=\"";
        ssResult += ssShortName;
        ssResult += "\"\r\nContent-Transfer-Encoding: base64";
        ssResult += "\r\nContent-Disposition: attachment;";
        ssResult += "\r\n\tfilename=\"";
        ssResult += ssShortName;
        ssResult += "\"";
        
    } else {
        
        if (ssContentType.GetLength()) {
            ssResult += ssContentType;        
        } else {
            ssResult += "text/plain";
        }
        
        ssResult += "; charset=\"us-ascii\"\r\n";
        ssResult += "Content-Transfer-Encoding: 7bit";
    }

    (* pssResult) = ssResult;
    
    return true;
}

bool CMimeMessage :: GetMessage(CString * pssMessage) const {
    
    CString ssBoundary;
    CString ssDeclareBoundary;
    CString ssSavedContentType;
    CString ssContentType;
    
    int i;

    if (! pssMessage)
        return false;
    
    // save content-type for mime body
    m_MimeHeader.Get("content-type", & ssSavedContentType);

    // get a new boundary string and build a content-type
    CMimeHeader :: GetBoundary(& ssBoundary);
    CMimeHeader :: GetBoundary(& ssDeclareBoundary, btBoundaryDeclare, & ssBoundary);
    ssContentType = "multipart/mixed; ";
    ssContentType.Append(ssDeclareBoundary);

    ((CMimeMessage *) this)->m_MimeHeader.Set("content-type", ssContentType);
    m_MimeHeader.Dump(pssMessage);

    // restore the existing content-type
    GetContentHeader(ssSavedContentType, "", & ssSavedContentType);
    ((CMimeMessage *) this)->m_MimeHeader.Set("content-type", ssSavedContentType);
    
    // append the message separator
    pssMessage->Append("\r\nThis is a multi-part message in MIME format.\r\n\r\n");
    // append the message boundary
    CMimeHeader :: GetBoundary(& ssDeclareBoundary, btBoundarySeparate, & ssBoundary);
    pssMessage->Append(ssDeclareBoundary);
    m_MimeHeader.Dump(pssMessage, false);
    // append the message body
    pssMessage->Append("\r\n");
    pssMessage->Append(m_MimeBody.GetBody());
    pssMessage->Append("\r\n\r\n");

    // attachments
    for (i = 0; i < (int) m_Attachments.GetSize(); i++)
    {
        if (! GetAttachment(m_Attachments[i], pssMessage, ssBoundary))
            return false;
    }
    
    CMimeHeader :: GetBoundary(& ssDeclareBoundary, btBoundaryTerminate, & ssBoundary);
    pssMessage->Append(ssDeclareBoundary);

    return true;
}

bool CMimeMessage :: GetAttachment(const CString& ssFilenameIn, CString * pssMessage, const CString& ssAttachmentBoundary) const {

    CString ssFileAttachmentName;
    CString ssFilename(ssFilenameIn);
    
    int dPos = ssFilename.InvPos('#');
    if (dPos >= 0) {        
        ssFilename.Mid(dPos + 1, ssFilename.GetLength(), & ssFileAttachmentName);
        ssFilename.Delete(dPos, ssFilename.GetLength());
    } else {
        ssFileAttachmentName = ssFilename;
    }
    
    CString ssAttachmentDeclareBoundary;
    CMimeBody smbMimeBody;
    CString ssContentType;
    CString ssBase64Encoded;
    
    if (! pssMessage)
        return false;
    
    if (! smbMimeBody.Load(ssFilename))
        return false;
    
    // attachment boundary
    CMimeHeader :: GetBoundary(& ssAttachmentDeclareBoundary, btBoundarySeparate, (CString *) & ssAttachmentBoundary);
    pssMessage->Append(ssAttachmentDeclareBoundary);
    
    // attachment content-type
    
    GetContentHeader("application/octet-stream", ssFileAttachmentName, & ssContentType);
    pssMessage->Append("content-type:");
    pssMessage->Append(ssContentType);
    
    // attachment body
    pssMessage->Append("\r\n\r\n");
    pssMessage->Append(CEncryption::Base64Encode(smbMimeBody.GetBody()));
    pssMessage->Append("\r\n\r\n");

    return true;
}


