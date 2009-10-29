/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef GLOBAL_STRINGS_HPP
#define GLOBAL_STRINGS_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

#define DECLARE_STRING(__strName, __strValue) static const char g_str##__strName[] = __strValue;

// general
DECLARE_STRING(SemiColSpace,                               "; "                                                          );
DECLARE_STRING(CommaSpace,                                 ", "                                                          );
DECLARE_STRING(ColSpace,                                   ": "                                                          );
DECLARE_STRING(DashDash,                                   "--"                                                          );
DECLARE_STRING(ColSlashSlash,                              "://"                                                         );
DECLARE_STRING(Name,                                       "NAME"                                                        );
DECLARE_STRING(Value,                                      "VALUE"                                                       );
DECLARE_STRING(CrLf,                                       "\r\n"                                                        );
DECLARE_STRING(Slash,                                      "/"                                                           );
DECLARE_STRING(Star,                                       "*"                                                           );
// Http specific
DECLARE_STRING(HttpOneOneSpace,                            "HTTP/1.1 "                                                   );
DECLARE_STRING(HttpSpaceOneZero,                           " HTTP/1.0"                                                   );
DECLARE_STRING(HttpGet,                                    "GET"                                                         );
DECLARE_STRING(HttpPost,                                   "POST"                                                        );
DECLARE_STRING(HttpOptions,                                "OPTIONS"                                                     );
DECLARE_STRING(HttpHead,                                   "HEAD"                                                        );
DECLARE_STRING(HttpPut,                                    "PUT"                                                         );
DECLARE_STRING(HttpDelete,                                 "DELETE"                                                      );
DECLARE_STRING(HttpTrace,                                  "TRACE"                                                       );
DECLARE_STRING(HttpConnect,                                "CONNECT"                                                     );
DECLARE_STRING(HttpHost,                                   "Host"                                                        );
DECLARE_STRING(HttpPrefix,                                 "HTTP_"                                                       );
DECLARE_STRING(HttpDate,                                   "Date"                                                        );
DECLARE_STRING(HttpSetCookie,                              "Set-cookie"                                                  );
DECLARE_STRING(HttpContentType,                            "Content-type"                                                );
DECLARE_STRING(HttpContentLength,                          "Content-length"                                              );
DECLARE_STRING(HttpAccept,                                 "Accept"                                                      );
DECLARE_STRING(HttpAcceptLanguage,                         "Accept-language"                                             );
DECLARE_STRING(HttpLocation,                               "Location"                                                    );
DECLARE_STRING(HttpConnection,                             "Connection"                                                  );
DECLARE_STRING(HttpKeepAlive,                              "keep-alive"                                                  );
DECLARE_STRING(HttpAuthorization,                          "Authorization"                                               );
DECLARE_STRING(HttpWWWAuthenticate,                        "WWW-Authenticate"                                            );
DECLARE_STRING(HttpIfModifiedSince,                        "if-Modified-Since"                                           );
DECLARE_STRING(HttpNTML,                                   "NTLM"                                                        );
DECLARE_STRING(HttpBasic,                                  "Basic "                                                      );
DECLARE_STRING(HttpBoundary,                               "boundary"                                                    );
DECLARE_STRING(HttpLastModified,                           "Last-Modified"                                               );
DECLARE_STRING(HttpUserAgent,                              "User-Agent"                                                  );
DECLARE_STRING(HttpEncApplicationXWWWFormUrlEncoded,       "application/x-www-form-urlencoded"                           );
DECLARE_STRING(HttpEncMultipartFormData,                   "multipart/form-data"                                         );
DECLARE_STRING(HttpTextHtml,                               "text/html"                                                   );
DECLARE_STRING(HttpSpecific,                               "http://"                                                     );
DECLARE_STRING(HttpDisallow,                               "Disallow"                                                    );
// CGI environment variables
DECLARE_STRING(CgiRequestMethod,                           "REQUEST_METHOD"                                              );
DECLARE_STRING(CgiServerPort,                              "SERVER_PORT"                                                 );
DECLARE_STRING(CgiServerName,                              "SERVER_NAME"                                                 );
DECLARE_STRING(CgiScriptName,                              "SCRIPT_NAME"                                                 );
DECLARE_STRING(CgiQueryString,                             "QUERY_STRING"                                                ); 
// HTTP return status
DECLARE_STRING(HttpStatus100,                              "100 Continue"                                                );
DECLARE_STRING(HttpStatus101,                              "101 Switching Protocols"                                     );
DECLARE_STRING(HttpStatus200,                              "200 OK"                                                      );
DECLARE_STRING(HttpStatus201,                              "201 Created"                                                 );
DECLARE_STRING(HttpStatus202,                              "202 Accepted"                                                );
DECLARE_STRING(HttpStatus203,                              "203 Non-Authoritative Information"                           );
DECLARE_STRING(HttpStatus204,                              "204 No Content"                                              );
DECLARE_STRING(HttpStatus205,                              "205 Reset Content"                                           );
DECLARE_STRING(HttpStatus206,                              "206 Partial Content"                                         );
DECLARE_STRING(HttpStatus207,                              "207 Partial Content-Length"                                  );
DECLARE_STRING(HttpStatus300,                              "300 Multiple Choices"                                        );
DECLARE_STRING(HttpStatus301,                              "301 Moved Permanently"                                       );
DECLARE_STRING(HttpStatus302,                              "302 Moved Temporarily"                                       );
DECLARE_STRING(HttpStatus303,                              "303 See Other"                                               );
DECLARE_STRING(HttpStatus304,                              "304 Not Modified"                                            );
DECLARE_STRING(HttpStatus305,                              "305 Use Proxy"                                               );
DECLARE_STRING(HttpStatus307,                              "307 Temporary Redirect"                                      );   //HTTP 1.1 Section 10.3.8
DECLARE_STRING(HttpStatus400,                              "400 Bad Request"                                             );
DECLARE_STRING(HttpStatus401,                              "401 Unauthorized"                                            );
DECLARE_STRING(HttpStatus402,                              "402 Payment Required"                                        );
DECLARE_STRING(HttpStatus403,                              "403 Forbidden"                                               );
DECLARE_STRING(HttpStatus404,                              "404 Not Found"                                               );
DECLARE_STRING(HttpStatus405,                              "405 Method Not Allowed"                                      );
DECLARE_STRING(HttpStatus406,                              "406 Not Acceptable"                                          );
DECLARE_STRING(HttpStatus407,                              "407 Proxy Authentication Required"                           );
DECLARE_STRING(HttpStatus408,                              "408 Request Time-out"                                        );
DECLARE_STRING(HttpStatus409,                              "409 Conflict"                                                );
DECLARE_STRING(HttpStatus410,                              "410 Gone"                                                    );
DECLARE_STRING(HttpStatus411,                              "411 Length Required"                                         );
DECLARE_STRING(HttpStatus412,                              "412 Precondition Failed"                                     );
DECLARE_STRING(HttpStatus413,                              "413 Request Entity Too Large"                                );
DECLARE_STRING(HttpStatus414,                              "414 Request-URI Too Large"                                   );
DECLARE_STRING(HttpStatus415,                              "415 Unsupported Media Type"                                  );
DECLARE_STRING(HttpStatus416,                              "416 Requested range not satisfiable"                         );   //HTTP 1.1 Section 10.4.17
DECLARE_STRING(HttpStatus417,                              "417 Expectation Failed"                                      );   //HTTP 1.1 Section 10.4.18
DECLARE_STRING(HttpStatus500,                              "500 Internal Server Error"                                   );
DECLARE_STRING(HttpStatus501,                              "501 Not Implemented"                                         );
DECLARE_STRING(HttpStatus502,                              "502 Bad Gateway"                                             );
DECLARE_STRING(HttpStatus503,                              "503 Service Unavailable"                                     );
DECLARE_STRING(HttpStatus504,                              "504 Gateway Time-out"                                        );
DECLARE_STRING(HttpStatus505,                              "505 HTTP Version Not Supported"                              );
DECLARE_STRING(HttpStatus921,                              "921 Socket Error"                                            );
DECLARE_STRING(HttpStatus922,                              "922 Connection Timed Out"                                    );
DECLARE_STRING(HttpStatus923,                              "923 Domain Name Resolution Failed"                           );
DECLARE_STRING(HttpStatus924,                              "924 Socket Bind Error"                                       );
DECLARE_STRING(HttpStatus925,                              "925 TLI Error"                                               );
DECLARE_STRING(HttpStatus926,                              "926 DNS queue full"                                          );
DECLARE_STRING(HttpStatus927,                              "927 DNS request timed out"                                   );
    
DECLARE_STRING(HttpStatusUnsupported,                      " Unsupported"                                                );
DECLARE_STRING(HttpCookieDateFormat,                       "$DayEnglish, $Day-$Month-$Year $Hour:$Min:$Sec"              );
DECLARE_STRING(HttpCookieScanDateFormat,                   "%s%c%d%s%d%d%c%d%c%d%s"                                      );
DECLARE_STRING(HttpCookieGmt,                              " GMT"                                                        );
DECLARE_STRING(HttpCookieExpires,                          "; expires="                                                  );
DECLARE_STRING(HttpCookiePath,                             "; path="                                                     );
DECLARE_STRING(HttpCookieDomain,                           "; domain="                                                   );
DECLARE_STRING(HttpCookieSecure,                           "; secure"                                                    );
DECLARE_STRING(HttpCookieScanExpires,                      "expires="                                                    );
DECLARE_STRING(HttpCookieScanPath,                         "path="                                                       );
DECLARE_STRING(HttpCookieScanDomain,                       "domain="                                                     );
DECLARE_STRING(HttpCookieScanSecure,                       "secure"                                                      );
DECLARE_STRING(Rfc822EmailAddressSpecials,                 "()/<>@,;:\\\".[]"                                            );
// HTML
DECLARE_STRING(Html_nbsp,                                  "nbsp"                                                        );
DECLARE_STRING(Html_agrave,                                "agrave"                                                      );
DECLARE_STRING(Html_aacute,                                "aacute"                                                      );
DECLARE_STRING(Html_acirc,                                 "acirc"                                                       );
DECLARE_STRING(Html_atilde,                                "atilde"                                                      );
DECLARE_STRING(Html_auml,                                  "auml"                                                        );
DECLARE_STRING(Html_aring,                                 "aring"                                                       );
DECLARE_STRING(Html_aelig,                                 "aelig"                                                       );
DECLARE_STRING(Html_ccedil,                                "ccedil"                                                      );
DECLARE_STRING(Html_egrave,                                "egrave"                                                      );
DECLARE_STRING(Html_eacute,                                "eacute"                                                      );
DECLARE_STRING(Html_ecirc,                                 "ecirc"                                                       );
DECLARE_STRING(Html_euml,                                  "euml"                                                        );
DECLARE_STRING(Html_igrave,                                "igrave"                                                      );
DECLARE_STRING(Html_iacute,                                "iacute"                                                      );
DECLARE_STRING(Html_icirc,                                 "icirc"                                                       );
DECLARE_STRING(Html_iuml,                                  "iuml"                                                        );
DECLARE_STRING(Html_eth,                                   "eth"                                                         );
DECLARE_STRING(Html_ntilde,                                "ntilde"                                                      );
DECLARE_STRING(Html_ograve,                                "ograve"                                                      );
DECLARE_STRING(Html_oacute,                                "oacute"                                                      );
DECLARE_STRING(Html_ocirc,                                 "ocirc"                                                       );
DECLARE_STRING(Html_otilde,                                "otilde"                                                      );
DECLARE_STRING(Html_ouml,                                  "ouml"                                                        );
DECLARE_STRING(Html_divide,                                "divide"                                                      );
DECLARE_STRING(Html_oslash,                                "oslash"                                                      );
DECLARE_STRING(Html_ugrave,                                "ugrave"                                                      );
DECLARE_STRING(Html_uacute,                                "uacute"                                                      );
DECLARE_STRING(Html_ucirc,                                 "ucirc"                                                       );
DECLARE_STRING(Html_uuml,                                  "uuml"                                                        );
DECLARE_STRING(Html_yacute,                                "yacute"                                                      );
DECLARE_STRING(Html_thorn,                                 "thorn"                                                       );
DECLARE_STRING(Html_yuml,                                  "yuml"                                                        );
DECLARE_STRING(Html_Agrave,                                "Agrave"                                                      );
DECLARE_STRING(Html_Aacute,                                "Aacute"                                                      );
DECLARE_STRING(Html_Acirc,                                 "Acirc"                                                       );
DECLARE_STRING(Html_Atilde,                                "Atilde"                                                      );
DECLARE_STRING(Html_Auml,                                  "Auml"                                                        );
DECLARE_STRING(Html_Aring,                                 "Aring"                                                       );
DECLARE_STRING(Html_AElig,                                 "AElig"                                                       );
DECLARE_STRING(Html_Ccedil,                                "Ccedil"                                                      );
DECLARE_STRING(Html_Egrave,                                "Egrave"                                                      );
DECLARE_STRING(Html_Eacute,                                "Eacute"                                                      );
DECLARE_STRING(Html_Ecirc,                                 "Ecirc"                                                       );
DECLARE_STRING(Html_Euml,                                  "Euml"                                                        );
DECLARE_STRING(Html_Igrave,                                "Igrave"                                                      );
DECLARE_STRING(Html_Iacute,                                "Iacute"                                                      );
DECLARE_STRING(Html_Icirc,                                 "Icirc"                                                       );
DECLARE_STRING(Html_Iuml,                                  "Iuml"                                                        );
DECLARE_STRING(Html_ETH,                                   "ETH"                                                         );
DECLARE_STRING(Html_Ntilde,                                "Ntilde"                                                      );
DECLARE_STRING(Html_Ograve,                                "Ograve"                                                      );
DECLARE_STRING(Html_Oacute,                                "Oacute"                                                      );
DECLARE_STRING(Html_Ocirc,                                 "Ocirc"                                                       );
DECLARE_STRING(Html_Otilde,                                "Otilde"                                                      );
DECLARE_STRING(Html_Ouml,                                  "Ouml"                                                        );
DECLARE_STRING(Html_times,                                 "times"                                                       );
DECLARE_STRING(Html_Oslash,                                "Oslash"                                                      );
DECLARE_STRING(Html_Ugrave,                                "Ugrave"                                                      );
DECLARE_STRING(Html_Uacute,                                "Uacute"                                                      );
DECLARE_STRING(Html_Ucirc,                                 "Ucirc"                                                       );
DECLARE_STRING(Html_Uuml,                                  "Uuml"                                                        );
DECLARE_STRING(Html_Yacute,                                "Yacute"                                                      );
DECLARE_STRING(Html_THORN,                                 "THORN"                                                       );
DECLARE_STRING(Html_szlig,                                 "szlig"                                                       );
DECLARE_STRING(Html_iexcl,                                 "iexcl"                                                       );
DECLARE_STRING(Html_cent,                                  "cent"                                                        );
DECLARE_STRING(Html_pound,                                 "pound"                                                       );
DECLARE_STRING(Html_curren,                                "curren"                                                      );
DECLARE_STRING(Html_yen,                                   "yen"                                                         );
DECLARE_STRING(Html_brvbar,                                "brvbar"                                                      );
DECLARE_STRING(Html_sect,                                  "sect"                                                        );
DECLARE_STRING(Html_uml,                                   "uml"                                                         );
DECLARE_STRING(Html_copy,                                  "copy"                                                        );
DECLARE_STRING(Html_ordf,                                  "ordf"                                                        );
DECLARE_STRING(Html_laquo,                                 "laquo"                                                       );
DECLARE_STRING(Html_not,                                   "not"                                                         );
DECLARE_STRING(Html_shy,                                   "shy"                                                         );
DECLARE_STRING(Html_reg,                                   "reg"                                                         );
DECLARE_STRING(Html_macr,                                  "macr"                                                        );
DECLARE_STRING(Html_deg,                                   "deg"                                                         );
DECLARE_STRING(Html_plusmn,                                "plusmn"                                                      );
DECLARE_STRING(Html_sup1,                                  "sup1"                                                        );
DECLARE_STRING(Html_sup2,                                  "sup2"                                                        );
DECLARE_STRING(Html_sup3,                                  "sup3"                                                        );
DECLARE_STRING(Html_acute,                                 "acute"                                                       );
DECLARE_STRING(Html_micro,                                 "micro"                                                       );
DECLARE_STRING(Html_para,                                  "para"                                                        );
DECLARE_STRING(Html_middot,                                "middot"                                                      );
DECLARE_STRING(Html_cedil,                                 "cedil"                                                       );
DECLARE_STRING(Html_ordm,                                  "ordm"                                                        );
DECLARE_STRING(Html_raquo,                                 "raquo"                                                       );
DECLARE_STRING(Html_frac14,                                "frac14"                                                      );
DECLARE_STRING(Html_frac12,                                "frac12"                                                      );
DECLARE_STRING(Html_frac34,                                "frac34"                                                      );
DECLARE_STRING(Html_iquest,                                "iquest"                                                      );
DECLARE_STRING(Html_quot,                                  "quot"                                                        );
DECLARE_STRING(Html_gt,                                    "gt"                                                          );
DECLARE_STRING(Html_lt,                                    "lt"                                                          );
DECLARE_STRING(Html_amp,                                   "amp"                                                         );
DECLARE_STRING(Html_SCRIPT,                                "SCRIPT"                                                      );
DECLARE_STRING(Html_OBJECT,                                "OBJECT"                                                      );
DECLARE_STRING(Html_STYLE,                                 "STYLE"                                                       );
DECLARE_STRING(Html_CSCRIPT,                               "/SCRIPT"                                                     );
DECLARE_STRING(Html_COBJECT,                               "/OBJECT"                                                     );
DECLARE_STRING(Html_CSTYLE,                                "/STYLE"                                                      ); 
// URL
DECLARE_STRING(Proto_HTTP,                                "HTTP"                                                         );
DECLARE_STRING(Proto_HTTPS,                               "HTTPS"                                                        );
DECLARE_STRING(Proto_FILE,                                "FILE"                                                         );
DECLARE_STRING(Proto_FTP,                                 "FTP"                                                          );
DECLARE_STRING(Proto_GOPHER,                              "GOPHER"                                                       );
DECLARE_STRING(Proto_NEWS,                                "NEWS"                                                         );
DECLARE_STRING(Proto_NNTP,                                "NNTP"                                                         );
DECLARE_STRING(Proto_TELNET,                              "TELNET"                                                       );
DECLARE_STRING(Proto_WAIS,                                "WAIS"                                                         );
DECLARE_STRING(Proto_PROSPERO,                            "PROSPERO"                                                     );
// internet
DECLARE_STRING(HostLocalAddress,                          "127.0.0.1"                                                    );
DECLARE_STRING(HostLocalName,                             "localhost"                                                    );

DECLARE_STRING(CMD_TRIM,                                  "TRIM"                                                         );
DECLARE_STRING(CMD_TRIM32,                                "TRIM32"                                                       );
DECLARE_STRING(CMD_LEFT,                                  "LEFT"                                                         );
DECLARE_STRING(CMD_RIGHT,                                 "RIGHT"                                                        );	
DECLARE_STRING(CMD_IS,                                    "IS"                                                           );
DECLARE_STRING(CMD_NOT,                                   "NOT"                                                          );
DECLARE_STRING(CMD_HAS,                                   "HAS"                                                          );
DECLARE_STRING(CMD_STARTS,                                "STARTS"                                                       );
DECLARE_STRING(CMD_ENDS,                                  "ENDS"                                                         );
DECLARE_STRING(CMD_UPCASE,                                "UPCASE"                                                       );
DECLARE_STRING(CMD_LCASE,                                 "LCASE"                                                        );
DECLARE_STRING(CMD_REVERSE,                               "REVERSE"                                                      );
DECLARE_STRING(CMD_REPLACE,                               "REPLACE"                                                      );
DECLARE_STRING(CMD_REPLACECI,                             "REPLACI"                                                      );
DECLARE_STRING(CMD_MORE,                                  "MORE"                                                         );
DECLARE_STRING(CMD_LESS,                                  "LESS"                                                         );
DECLARE_STRING(CMD_URLENCODE,                             "URLENCODE"                                                    );
DECLARE_STRING(CMD_URLDECODE,                             "URLDECODE"                                                    );
DECLARE_STRING(CMD_HTMLQUOTE,                             "HTMLQUOTE"                                                    );
DECLARE_STRING(CMD_HTMLDEQUOTE,                           "HTMLDEQUOTE"                                                  );
DECLARE_STRING(CMD_URLSCH,                                "URLSCH"                                                       );   // scheme, http
DECLARE_STRING(CMD_URLHOST,                               "URLHOST"                                                      );   // server name
DECLARE_STRING(CMD_URLDIR,                                "URLDIR"                                                       );   // directory with no parameters, cannot be emtpy
DECLARE_STRING(CMD_URLFILE,                               "URLFILE"                                                      );   // filename with no parameters
DECLARE_STRING(CMD_URLARG,                                "URLARG"                                                       );   // everything after the parameter (?) without the location inside page
DECLARE_STRING(CMD_CLEFT,                                 "CLEFT"                                                        );
DECLARE_STRING(CMD_CRIGHT,                                "CRIGHT"                                                       );
DECLARE_STRING(True,                                      "true"                                                         );
DECLARE_STRING(False,                                     "false"                                                        );
DECLARE_STRING(One,                                       "1"                                                            );
DECLARE_STRING(Zero,                                      "0"                                                            );
DECLARE_STRING(Yes,                                       "yes"                                                          );
DECLARE_STRING(No,                                        "no"                                                           );
DECLARE_STRING(Y,                                         "y"                                                            );
DECLARE_STRING(N,                                         "n"                                                            );
DECLARE_STRING(SKb,                                       " Kb"                                                          );
DECLARE_STRING(SMb,                                       " Mb"                                                          );
DECLARE_STRING(SGb,                                       " Gb"                                                          );
// Dates
DECLARE_STRING(DateDayEnglish,                            "DayEnglish"                                                   );
DECLARE_STRING(DateDayEng,                                "DayEng"                                                       );
DECLARE_STRING(DateDayFrench,                             "DayFrench"                                                    );
DECLARE_STRING(DateDayFre,                                "DayFre"                                                       );
DECLARE_STRING(DateMonthFrench,                           "MonthFrench"                                                  );
DECLARE_STRING(DateMonthEnglish,                          "MonthEnglish"                                                 );
DECLARE_STRING(DateMonthEng,                              "MonthEng"                                                     );
DECLARE_STRING(DateMonthFre,                              "MonthFre"                                                     );
DECLARE_STRING(DateYear,                                  "Year"                                                         );
DECLARE_STRING(DateDay,                                   "Day"                                                          );
DECLARE_STRING(DateMonth,                                 "Month"                                                        );
DECLARE_STRING(DateHour,                                  "Hour"                                                         );
DECLARE_STRING(DateMin,                                   "Min"                                                          );
DECLARE_STRING(DateSec,                                   "Sec"                                                          );
DECLARE_STRING(DateSDay,                                  " day"                                                         );
DECLARE_STRING(DateSHour,                                 " hour"                                                        );
DECLARE_STRING(DateSMinute,                               " minute"                                                      );
DECLARE_STRING(DateSSecond,                               " second"                                                      );
DECLARE_STRING(DateSDays,                                 " days"                                                        );
DECLARE_STRING(DateSHours,                                " hours"                                                       );
DECLARE_STRING(DateSMinutes,                              " minutes"                                                     );
DECLARE_STRING(DateSSeconds,                              " seconds"                                                     );
// paths
DECLARE_STRING(PathCurrent,                               "."                                                            );
DECLARE_STRING(PathParent,                                ".."                                                           );
 
#endif
