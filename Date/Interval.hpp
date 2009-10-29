/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    _____________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_INTERVAL_HPP
#define BASE_INTERVAL_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>

typedef enum {
	itMicroseconds, itMilliseconds, itSeconds, itMinutes, itHours
} CIntervalType;

class CInterval : public CObject {
#ifdef _UNIX
	property(struct timeval, Start);
#endif
#ifdef _WIN32
	property(struct base_timeb, Start);
#endif
public:
	CInterval(void);
	virtual ~CInterval(void);
	long Get(CIntervalType = itMilliseconds) const;
	void Reset(void);
};

#endif
