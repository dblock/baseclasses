/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-2001 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#ifndef EMAIL_MIMEMESSAGE_HPP
#define EMAIL_MIMEMESSAGE_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>

#include "mimebody.hpp"
#include "mimeheader.hpp"

class CMimeMessage {
    property(CMimeHeader, MimeHeader);
    property(CMimeBody, MimeBody);
    property(CVector<CString>, Attachments);
    property(CString, Status);
public:    
    bool GetMessage(CString * pssMessage) const;
    bool GetAttachment(const CString& ssFilename, CString * pssMessage, const CString& ssAttachmentBoundary) const;
    bool GetContentHeader(const CString& ssContentType, const CString& ssFilePath, CString * pssResult) const;    
    CMimeMessage(void);
    ~CMimeMessage(void);
};

#endif
