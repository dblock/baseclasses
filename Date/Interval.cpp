/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    _____________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Interval.hpp"

CInterval::CInterval(void) {
	Reset();
}

CInterval::~CInterval(void) {

}

void CInterval::Reset(void) {
       #ifdef _UNIX
	gettimeofday(&m_Start, NULL);
       #endif
       #ifdef _WIN32
	base_ftime(&m_Start);
       #endif
}

long CInterval::Get(CIntervalType IntervalType) const {
       #ifdef _UNIX
	struct timeval m_Current;
	gettimeofday(&m_Current, NULL);
	switch(IntervalType) {
	case itMicroseconds:
		return (long) (1000000.0 * (m_Current.tv_sec - m_Start.tv_sec) + (m_Current.tv_usec - m_Start.tv_usec));
	case itMilliseconds:
		return (long) (1000.0 * (m_Current.tv_sec - m_Start.tv_sec) + (m_Current.tv_usec - m_Start.tv_usec)/1000);
	case itSeconds:
		return (long) ((m_Current.tv_sec - m_Start.tv_sec) + (m_Current.tv_usec - m_Start.tv_usec)/1000000);
	case itMinutes:
		return (long) ((m_Current.tv_sec - m_Start.tv_sec)/60);
	case itHours:
		return (long) ((m_Current.tv_sec - m_Start.tv_sec)/3600);
	}
       #endif
       #ifdef _WIN32
	struct base_timeb m_Current;
	base_ftime(&m_Current);
	switch(IntervalType) {
	case itMicroseconds:
		return (long) (1000000.0 * (m_Current.time - m_Start.time) + (m_Current.millitm - m_Start.millitm)*1000);
	case itMilliseconds:
		return (long) (1000.0 * (m_Current.time - m_Start.time) + (m_Current.millitm - m_Start.millitm));
	case itSeconds:
		return (long) ((m_Current.time - m_Start.time) + (m_Current.millitm - m_Start.millitm)/1000);
	case itMinutes:
		return (long) ((m_Current.time - m_Start.time)/60);
	case itHours:
		return (long) ((m_Current.time - m_Start.time)/3600);
	}
       #endif
	return -1;
}

