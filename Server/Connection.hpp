/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_CONNECTION_HPP
#define BASE_CONNECTION_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>
#include <Socket/Socket.hpp>

class CConnection : public CObject {
private:
	readonly_property(CSocket, Socket);
	copy_property(void *, Server);
	inline void MakePeerInformation(void) { m_Socket.MakePeerInformation(); }
public:
	inline void Close(void) { m_Socket.Close(); }
	inline int Write(const CString& Value) const { return m_Socket.Write(Value); }
	inline int WriteLine(const CString& Value) const { return m_Socket.WriteLine(Value); }
	inline int ReadLine(CString * pResult) const { return m_Socket.ReadLine(pResult); }
	inline int Read(CString * pResult, const int MaxLength = -1) const { return m_Socket.Read(pResult, MaxLength); }
	virtual void Execute(void *);
	CConnection(void);
	virtual ~CConnection(void);
        inline void SetPeerSocket(int PeerSocket) { m_Socket.SetPeerSocket(PeerSocket); }
        inline void SetPeerAddrIn(struct sockaddr_in PeerAddrIn) { m_Socket.SetPeerAddrIn(PeerAddrIn); }
        inline void SetTimeout(int Timeout) { m_Socket.SetTimeout(Timeout); }
};

#endif
