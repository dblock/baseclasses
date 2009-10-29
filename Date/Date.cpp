/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    _____________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    18.03.2000: not using sprintf
    
*/

#include <baseclasses.hpp> 
#include "Date.hpp"
#include <String/GStrings.hpp>
#include <String/EStrings.hpp>

static char * DateMonthsFrench[] = {
  "Janvier", 
  "Fevrier",
  "Mars",
  "Avril",
  "Mai",
  "Juin",
  "Juillet", 
  "Août",
  "Septembre",
  "Octobre",
  "Novembre",
  "Décembre"
};

static char * DateMonthsEnglish[] = {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

static char * DateDaysFrench[] = { "Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi" };
static char * DateDaysFrenchShort[] = { "Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam" };
static char * DateDaysEnglish[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static char * DateDaysEnglishShort[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static char * DateMonthsEnglishShort[] = {"Jan","Feb","Mar","Apr", "May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
static char * DateMonthsFrenchShort[] = {"Jan","Fev","Mar","Avr","Mai","Jun","Jul","Aoû","Sep","Oct","Nov","Déc"};

const char * CDate::Day(CLocale Locale, int Day) const {
  assert((Day >= 0)&&(Day < 7));
  switch(Locale) {
  case LocEnglish:
    return DateDaysEnglish[Day];
  case LocFrench:
    return DateDaysFrench[Day];
  }
  return "";
}

const char * CDate::DayShort(CLocale Locale, int Day) const {
  assert((Day >= 0)&&(Day < 7));
  switch(Locale) {
  case LocEnglish:
    return DateDaysEnglishShort[Day];
  case LocFrench:
    return DateDaysFrenchShort[Day];
  }
  return "";
}


const char * CDate::Month(CLocale Locale, int Mon) const {
  assert((Mon >= 0)&&(Mon < 12));
  switch(Locale) {
  case LocEnglish:
    return DateMonthsEnglish[Mon];
  case LocFrench:
    return DateMonthsFrench[Mon];
  }
  return "";
}

const char * CDate::MonthShort(CLocale Locale, int Mon) const {
  assert((Mon >= 0)&&(Mon < 12));
  switch(Locale) {
  case LocEnglish:
    return DateMonthsEnglishShort[Mon];
  case LocFrench:
    return DateMonthsFrenchShort[Mon];
  }
  return "";
}

CDate::CDate(struct tm * NewTime){
  if (NewTime) {
    memcpy(&m_TmTime, NewTime, sizeof(m_TmTime));
    m_TTime = mktime(&m_TmTime);
  } else {
    time(&m_TTime);
    base_localtime(m_TTime, m_TmTime);
  }
}

CDate::CDate(void){
  time(&m_TTime);
  base_localtime(m_TTime, m_TmTime);
}

void CDate::GetVariable(CString& Term, int /* Dummy */ ) const {
  if (Term.Same(g_strDateDayEnglish)) Term = Day(LocEnglish);
  else if (Term.Same(g_strDateDayEng)) Term = DayShort(LocEnglish);
  else if (Term.Same(g_strDateDayFrench)) Term = Day(LocFrench);
  else if (Term.Same(g_strDateDayFre)) Term = DayShort(LocFrench);
  else if (Term.Same(g_strDateMonthFrench)) Term = Month(LocFrench);
  else if (Term.Same(g_strDateMonthEnglish)) Term = Month(LocEnglish);
  else if (Term.Same(g_strDateMonthEng)) Term = MonthShort(LocEnglish);
  else if (Term.Same(g_strDateMonthFre)) Term = MonthShort(LocFrench);
  else if (Term.Same(g_strDateYear)) MapYear(Term);
  else if (Term.Same(g_strDateDay)) MapDay(Term);
  else if (Term.Same(g_strDateMonth)) MapMonth(Term);
  else if (Term.Same(g_strDateHour)) { Term.Empty(); if (m_TmTime.tm_hour < 10) Term = g_strZero; Term += CString::IntToStr(m_TmTime.tm_hour); }
  else if (Term.Same(g_strDateMin)) { Term.Empty(); if (m_TmTime.tm_min < 10) Term = g_strZero; Term += CString::IntToStr(m_TmTime.tm_min); }
  else if (Term.Same(g_strDateSec)) { Term.Empty(); if (m_TmTime.tm_sec < 10) Term = g_strZero; Term += CString::IntToStr(m_TmTime.tm_sec); }
}

CString CDate::Map(const CString& Expression) const {
  CString Tmp;
  Map(Expression, Tmp);
  return Tmp;
}

CString& CDate::Map(const CString& Expression, CString& Target) const {
  MapTerm(Expression, Target, 0);
  return Target;
}

CString& CDate::MapTerm(const CString& Source, CString& Target, int Dummy) const {
  MAP_TERM_MACRO(Source, Target, MapTerm, GetVariable, false, Dummy);	
}

bool CDate::EncodeSimpleDate(const CString& Date, struct tm& Result, bool bUsEncoding) {
  memset(& Result, 0, sizeof(struct tm));
  if ((unsigned int) Date.GetLength() == sizeof("DDMMYY") - 1) {
    if (bUsEncoding) Result.tm_mday = Date.GetInt(2, 2); else Result.tm_mday = Date.GetInt(0, 2);
    if (bUsEncoding) Result.tm_mon = Date.GetInt(0, 2) - 1; else Result.tm_mon = Date.GetInt(2, 2) - 1;
    Result.tm_year = Date.GetInt(4, 2);
    if (Result.tm_year < 70)
      Result.tm_year += 100;
  } else if ((unsigned int) Date.GetLength() == sizeof("DDMMYYYY") - 1) {
    if (bUsEncoding) Result.tm_mday = Date.GetInt(2, 2); else Result.tm_mday = Date.GetInt(0, 2);
    if (bUsEncoding) Result.tm_mon = Date.GetInt(0, 2) - 1; else Result.tm_mon = Date.GetInt(2, 2) - 1;
    Result.tm_year = Date.GetInt(4, 4) - 1900;
  } else if ((unsigned int) Date.GetLength() == sizeof("DD/MM/YY") - 1) {
    if (bUsEncoding) Result.tm_mday = Date.GetInt(3, 2); else Result.tm_mday = Date.GetInt(0, 2);
    if (bUsEncoding) Result.tm_mon = Date.GetInt(0, 2) - 1; else Result.tm_mon = Date.GetInt(3, 2) - 1;
    Result.tm_year = Date.GetInt(6, 2);
    if (Result.tm_year < 70)
      Result.tm_year += 100;
  } else if ((unsigned int) Date.GetLength() == sizeof("DD/MM/YYYY") - 1) {
    if (bUsEncoding) Result.tm_mday = Date.GetInt(3, 2); else Result.tm_mday = Date.GetInt(0, 2);
    if (bUsEncoding) Result.tm_mon = Date.GetInt(0, 2) - 1; else Result.tm_mon = Date.GetInt(3, 2) - 1;
    Result.tm_year = Date.GetInt(6, 4) - 1900;
  } else {
    return false;
  }
  Result.tm_hour = 0;
  Result.tm_min = 0;
  Result.tm_sec = 0;

  Trace(tagDateTime, levVerbose, ("CDate:: EncodeSimpleDate - %d:%d:%d", Result.tm_mon, Result.tm_mday, Result.tm_year));

  return true;
}

int CDate::CompareDates(const CString& First, const CString& Second) {
  struct tm FirstDate;
  EncodeDate(First, FirstDate);
  struct tm SecondDate;
  EncodeDate(Second, SecondDate);
  time_t Time1 = mktime(&FirstDate);
  time_t Time2 = mktime(&SecondDate);
  return Time1 - Time2;
}

bool CDate::EncodeDate(
    const CString& DateString, 
    const CString& DateFormat, 
    struct tm& Result) {

  CString Date(DateString);
  
  static const char * Months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  static const char * WeekDays[] = {"Sun", "Mon","Tue","Wed","Thu","Fri","Sat"};
  static const char * Zones[] = {"UTC", "GMT", "EDT", "EST", "CDT", "CST", "MDT", "MST", "PDT", "PST"};
  static const int ZonesHoursSkew[] = {0, 0, 4, 5, 5, 6, 6, 7, 7, 8};
	
  CString WeekDay;
  CString Month;
  CString Zone;
  
  Date.Replace('-', ' ');

  memset(&Result, 0, sizeof(Result));

  bool fScan = 
      SSScan(
          Date, 
          &DateFormat, 
          &WeekDay, 
          NULL, 
          &Result.tm_mday, 
          &Month, 
          &Result.tm_year, 
          &Result.tm_hour, 
          NULL, 
          &Result.tm_min, 
          NULL, 
          &Result.tm_sec, 
          &Zone);
  
  Trace(tagDateTime, levVerbose, 
        ("CDate:: EncodeDate - scanned %s with " \
         "WeekDay: %s " \
         "Day: %d " \
         "Month: %s " \
         "Year: %d " \
         "Hour: %d " \
         "Minute: %d " \
         "Second: %d " \
         "Zone: %s", 
         Date.GetBuffer(),
         WeekDay.GetBuffer(),
         Result.tm_mday,
         Month.GetBuffer(),
         Result.tm_year,
         Result.tm_hour,
         Result.tm_min,
         Result.tm_sec,
         Zone.GetBuffer()));
  
  if (fScan) {

		int i;
      
		if (Result.tm_year > 1900) 
			Result.tm_year-=1900;
    
		for (i = 0; i < BASE_DIM(Months); i++) {
			if (Month.Same(Months[i])) {
				Result.tm_mon = i;
				break;
			}
		}

		for (i = 0; i < BASE_DIM(WeekDays); i++) {
			// support full weekdays as well
			if (WeekDay.StartsWithSame(WeekDays[i])) {
				Result.tm_wday = i;
				break;
			}
		}

		// time zone adjustments
		int ZoneAdjustment = 0;

		if (Zone.IsInt( & ZoneAdjustment)) {
			ZoneAdjustment *= 3600;
		} else {
			for (i = 0; i < BASE_DIM(Zones); i++) {
				if (Zone.Same(Zones[i])) {
					ZoneAdjustment = ZonesHoursSkew[i] * 3600;
					break;
				}
			}
		}

		Trace(tagDateTime, levVerbose, ("CDate:: EncodeSimpleDate - ZoneAdjustment %d second(s)", ZoneAdjustment));

		Result.tm_isdst = 0;

		if (ZoneAdjustment) {
			time_t AdjustedTime;
#ifdef _UNIX
			base_gmtime(AdjustedTime, Result);
#endif
#ifdef _WIN32
			_tzset();
			AdjustedTime = mktime(&Result);
			AdjustedTime -= _timezone;
#endif
			AdjustedTime -= ZoneAdjustment;
			base_localtime(AdjustedTime, Result);
		}

		// Result.tm_gmtoff = ZoneAdjustment;    
		return true;
  }
  return false;
}

bool CDate::EncodeDate(const CString& DateString, struct tm& Result) {
  // date format: Fri, 16 Jul 1999 15:35:06 GMT
  if (!EncodeDate(DateString, g_strHttpCookieScanDateFormat, Result)) {
    time_t TimeT;
    time(&TimeT);
    base_localtime(TimeT, Result);
    return false;
  }
  return true;
}

CString CDate::GetElapsedTime(const long After, const long Before) {
  long Seconds = abs(After - Before);
  long Minutes = Seconds / 60; Seconds %= 60;
  long Hours = Minutes / 60; Minutes %= 60;
  long Days = Hours / 24; Hours %= 24;
  CString Result;
  if (Days) {
    Result = CString::IntToStr(Days);
    Result += (Days > 1) ? g_strDateSDays : g_strDateSDay;    
    Result += ' ';
  }
  if (Hours) {
    Result += CString::IntToStr(Hours);
    Result += (Hours > 1) ? g_strDateSHours : g_strDateSHour;    
    Result += ' ';
  }
  if (Minutes) {
    Result += CString::IntToStr(Minutes);
    Result += (Minutes > 1) ? g_strDateSMinutes : g_strDateSMinute;    
    Result += ' ';
  }
  if (Seconds) {
    Result += CString::IntToStr(Seconds);
    Result += (Seconds > 1) ? g_strDateSSeconds : g_strDateSSecond;
  }
  if (!Result.GetLength()) {
    Result = '0';
    Result += g_strDateSSeconds;
  }
  return Result;
}

CString CDate::GetElapsedTime(const CDate& After, const CDate& Before) {
  return GetElapsedTime(After.m_TTime, Before.m_TTime);
}

long int CDate::funct1(int y, int m) {
	long int result;
	if (m <= 2) y -= 1;
	result = y;
	return (result);
}

long int CDate::funct2 (int m) {
	long int result;
	if ( m <= 2 ) result=m+13;
	else result = m + 1;
	return(result);
}

long int CDate::DayCount(int m, int d, int y) {
	return 1461 *  funct1(y,m) / 4 + 153 * funct2(m) / 5 + d;
}

bool CDate::ctime_r(const time_t * Clock, CString& Result) {
#if defined(HAVE_CTIME_R_SIZE) || defined(HAVE_CTIME_R_POSIX)
	char Buffer[KBYTE];
	char * ResultPtr = ::ctime_r(Clock, (char *) Buffer
       #ifdef HAVE_CTIME_R_SIZE
        , KBYTE
       #endif
        );
	if (ResultPtr) {
		Result = ResultPtr;
		Result.Trim32();
	}	
	return (ResultPtr != NULL);
#else // _WIN32 or neither POSIX nor BSD
	Result = ctime(Clock);
	Result.Trim32();
	return true;
#endif
}

bool CDate::asctime_r(const struct tm * Tm, CString& Result) {
#if defined(HAVE_CTIME_R_SIZE) || defined(HAVE_CTIME_R_POSIX)
	char Buffer[KBYTE];
	char * ResultPtr = ::asctime_r(Tm, Buffer
       #ifdef HAVE_CTIME_R_SIZE
        , KBYTE
       #endif
        );
	if (ResultPtr) {
		Result = ResultPtr;
		Result.Trim32();
	}
	return (ResultPtr != NULL);
#else // _WIN32, not POSIX nor BSD
	Result = asctime(Tm);
	Result.Trim32();
	return true;
#endif
}
