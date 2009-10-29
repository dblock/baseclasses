/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_HTTP_INPUT_HPP
#define BASE_HTTP_INPUT_HPP

#include <platform/include.hpp>
#include <Io/Io.hpp>
#include <String/StringPair.hpp>

typedef enum CHttpIoType { htUnknown, htGet, htPost, htHead, htPut, htDelete, htTrace, htConnect, htOptions };

#include <Internet/Url.hpp>
#include <String/StringTable.hpp>
#include <HashTable/VectorTable.hpp>
#include <List/List.hpp>
#include <Internet/Cookie.hpp>
#include <Mutex/Atomic.hpp>
#include <Tree/XmlTree.hpp>
    
class CConnection;

typedef enum { 
	hftNone = 0, 
	hftImage = 1, 
	hftDefault = 2, 
	hftDocument = 4, 
	hftScript = 8,
	hftStylesheet = 16
} CHttpIoFileContents;

typedef struct _CHttpIoFileTypes {
    char * Extension;
    char * MimeType;
	unsigned int MimeContents;
} CHttpIoFileTypes;

class CHttpIo : public CIo {
	/* output buffer */
	property(bool, BufferLock);
	property(CString, Buffer);
	/* request and post fields */
	property(CVectorTable<CStringTable>, HttpFields); // requested HTTP fields
	property(CVectorTable<CStringTable>, PostFields); // posted HTTP fields
	property(CStringTable, GetFields);
	property(CHttpIoType, InputType);
	copy_property(CConnection *, Connection);
	/* request */
	property(CString, RequestVersion);
	property(CString, RequestMethodString);
	property(bool, Malformed);
	property(CUrl, RequestUrl);
	property(CVector<CString>, Arguments);
	/* cookies */
	property(CList<CCookie>, Cookies);
	/* output */
	property(int, RStatus);
	property(CStringTable, RFields);
    property(bool, SendHeaders);
    property(bool, SendContentLength);
    property(bool, KeepAlive);
    property(CString, ErrorFooter);
public:
	static CAtomic m_TotalTraffic;
	static CString GetRStatusString(const int);
	static CString HTTPIoTypeToStr(const CHttpIoType&);
	static CHttpIoType StrToHTTPIoType(const CString&);
public:
	const CCookie * GetCookie(const CString&) const;
	bool RemoveCookie(const CString&);
	bool SetCookie(const CString&, const CString&);
	bool SetRContentType(const CString& Filename);
private:
	bool ProcessCookies(void);
private:
	bool ProcessRequest(void);
	bool ProcessMethod(void);
	bool ProcessOptions(void);
	bool ProcessPostMethod(void);
	bool ProcessGetMethod(void);
	// post parsing
	bool ProcessPost_Multipart_form_data(void);
	bool ProcessPost_Application_x_www_form_urlencoded(void);
	int RemoteRead(CString *, int) const;
    void Initialize(void);
public:
	void Flush(void);
	CHttpIo(void);
	CHttpIo(const CConnection&);
	virtual ~CHttpIo(void);
	inline bool IsPost(void) const { return (m_InputType == htPost); }
	inline bool IsMultipart(void) const { return m_HttpFields.FindElement("Content-Type").GetValue("Content-Type").Same("multipart/form-data"); }
	inline bool IsGet(void) const { return (m_InputType == htGet); }
	inline bool IsMalformed(void) const { return m_Malformed; }
	inline bool IsWellFormed(void) const { return !m_Malformed; }
public:
    void Send(const CString& Filename);
    void SendFileContents(const CString& Filename, const CString& Contents);
	inline void Write(const CString& Value) { if (!m_BufferLock) m_Buffer.Append(Value); }
	inline void WriteLine(const CString& Value) { if (!m_BufferLock) m_Buffer.Append(Value); if (Value.EndsWith("\n")) if (!m_BufferLock) m_Buffer.Append("\n"); }
public:
	/* form posts */
	bool PostReplace(const CString& Source, const CString& Target);
	inline bool GetGetContains(const CString& Group) const { return (m_GetFields.Contains(Group)); }
	inline bool PostGetContains(const CString& Group) const { return (m_PostFields.FindElementIndex(Group) != -1); }
	bool PostGetContains(const CString& Group, const CString& Value) const;
	bool PostGetCopy(const CString& Group, const CString& Value, CString& Target) const;
	inline CString PostGet(const CString& Group, const CString& Value) const { return m_PostFields.FindElement(Group).GetValue(Value); }
	inline CString PostGet(const CString& Group) const { return m_PostFields.FindElement(Group).GetValue("Value"); }
	inline int PostGetSize(void) const { return m_PostFields.GetSize(); }
	inline CString PostGetValueAt(const int Index) const { return m_PostFields.GetElementAt(Index).GetValue("Value"); }
	inline CString PostGetNameAt(const int Index) const { return m_PostFields.GetKeyAt(Index); }
	inline CString GetGet(const CString& Name) const { return m_GetFields.GetValue(Name); }
	inline int GetGetSize(void) const { return m_GetFields.GetSize(); }
	inline CString GetGetValueAt(const int Index) const { return m_GetFields.GetValueAt(Index); }
	inline CString GetGetNameAt(const int Index) const { return m_GetFields.GetNameAt(Index); }
	void PostSetNameValue(const CString& Name, const CString& Value);
public:	
    bool GetUsernamePassword(CString& Username, CString& Password);
    void DenyAccess(const CString& Realm);
    bool PopulateXmlNode(CXmlTree& Tree, CTreeElement< CXmlNode > * pXmlNode);
	static bool GetContentTypeExtension(const CString& ContentType, const CHttpIoFileTypes ** pType);
	CString GetDefaultDocument(void) const;
};

#endif
