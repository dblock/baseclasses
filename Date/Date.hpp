/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    23.09.1999: reentrancy support, localtime_r, etc., fixed Encode*Date
    
*/

#ifndef C_DATE_HPP
#define C_DATE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

typedef enum { LocEnglish, LocFrench } CLocale;

class CDate : public CObject {
	readonly_property(time_t, TTime);
	copy_property(struct tm, TmTime);
private:
	CString& MapTerm(const CString& Source, CString& Target, int Dummy) const;
public:
	CDate(void);
	CDate(struct tm *);
	static bool ctime_r(const time_t * /* Clock */, CString& /* Result */);	
	static bool asctime_r(const struct tm * /* Tm */, CString& /* Result */);
	inline CString AscTime(void) const { CString Result; CDate::asctime_r(&m_TmTime, Result); return Result; }	
	CString& Map(const CString& Expression, CString& Target) const;
	inline CString& MapYear(CString& Target) const { Target = CString::IntToStr(m_TmTime.tm_year + 1900); return Target; }
	inline CString& MapMonth(CString& Target) const { Target = CString::IntToStr(m_TmTime.tm_mon + 1, 2); return Target; }
	inline CString& MapDay(CString& Target) const { Target = CString::IntToStr(m_TmTime.tm_mday, 2); return Target; }
	CString Map(const CString&) const;
	void GetVariable(CString&, int Dummy) const;

	inline const char * Day(CLocale Locale = LocEnglish) const { return Day(Locale, DayOfWeek()); }
	inline const char * DayShort(CLocale Locale = LocEnglish) const { return DayShort(Locale, DayOfWeek()); }
	const char * Day(CLocale, int) const;
	const char * DayShort(CLocale, int) const;
	inline const char * Month(CLocale Locale = LocEnglish) const { return Month(Locale, m_TmTime.tm_mon); }
	inline const char * MonthShort(CLocale Locale = LocEnglish) const { return MonthShort(Locale, m_TmTime.tm_mon); }
	const char * Month(CLocale, int) const;
	const char * MonthShort(CLocale, int) const;
  
	inline int DayOfWeek(void) const { return m_TmTime.tm_wday; }
private:
	static long int CDate::funct1(int y, int m);
	static long int CDate::funct2(int m);
public:
	static long int CDate::DayCount(int m, int d, int y);
	static bool EncodeDate(const CString& /* Date */, struct tm& /* Result */);
    static bool EncodeDate(const CString& DateString, const CString& DateFomat, struct tm& Result);
	static bool EncodeSimpleDate(const CString& /* Date */, struct tm& /* Result */, bool bUsEncoding = false);
	static int CompareDates(const CString&, const CString&);
	static CString GetElapsedTime(const CDate& After, const CDate& Before);
	static CString GetElapsedTime(const long, const long);
};

#endif
