/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_WEB_SERVER_HPP
#define BASE_WEB_SERVER_HPP

#include <platform/platform.hpp>
#include <Server/Server.hpp>
#include <Io/HttpIo.hpp>

class CWebServer : public CServer {
private:
	void ExecuteClient(const CConnection&);
public:
	CWebServer(const int, const CString&);
	virtual void ExecuteWebClient(CHttpIo&);
};

#endif
