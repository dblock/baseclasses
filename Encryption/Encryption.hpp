/*

    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_ENCRYPTION_H
#define BASE_ENCRYPTION_H

#include <platform/include.hpp>
#include <String/String.hpp>

#define BASE64_ENCODE_REQSIZE(__szSize) (__szSize + ((__szSize + 3) / 3) + 4)
#define BASE64_DECODE_REQSIZE(__szSize) (((__szSize + 3) / 4) * 3 + 1)

class CEncryption {
public:
    CEncryption(void);
    virtual ~CEncryption(void) = 0;
    // base 64
    static CString Base64Encode(const CString& String);
    static CString Base64Decode(const CString& String);
    // standard encryption key, multiplier and prime
    static CString Encrypt(const CString& String, const int Key = 1234, const long int C1 = 52845, const long int C2 = 11719);
    static CString Decrypt(const CString& String, const int Key = 1234, const long int C1 = 52845, const long int C2 = 11719);
private:
    static bool CryptBase64Encode(const char * lpszString, char * lpszBuffer, int * dwSize, int dwSourceSize);
    static bool CryptBase64Decode(const char * lpszString, char * lpszBuffer, int * dwSize, int dwSourceSize); 
};

#endif
