/*

  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________
  
    written by Daniel Doubrovkine - dblock@vestris.com
    
*/

#include <baseclasses.hpp>
#include "Encryption.hpp"

CString CEncryption::Encrypt(const CString& String, const int Key, const long int C1, const long int C2) {
    CString Result;
    int LKey = Key;
    if (String.GetLength()) {
        signed char t;
        char * tStr = new char[String.GetLength() * 2 + 1];
        for (register int i=0;i<(int)String.GetLength();i++) {
            t = (signed char) (((signed char)String[i]) ^ (LKey >> 8));
            tStr[2*i] = (char) ((t & 15) + 64);
            tStr[2*i+1] = (char) (((t & 240) >> 4) + 64);
            LKey = (t + LKey) * C1 + C2;
        }
        tStr[String.GetLength() * 2] = 0;
        Result = tStr;
        delete[] tStr;
    }
    return Result;
}

CString CEncryption::Decrypt(const CString& String, const int Key, const long int C1, const long int C2) {
    CString Result;
    if (String.GetLength()){
        signed char t;
        int LKey = Key;
        int Len = String.GetLength() / 2;
        char * tStr = new char[Len+1];
        for (register int i=0; i< Len; i++){
            t = (signed char) ((((signed char)String[2*i]) - 64)  + ((((signed char)String[2*i+1]) - 64) << 4));
            tStr[i] = (char) (t ^ (LKey >> 8));
            if (!tStr[i]) {
                Len = i;
                break;
            }
            LKey = (t + LKey) * C1 + C2;
        }
        tStr[Len] = 0;
        Result = tStr;
        delete[] tStr;
    }
    return Result;
}

static signed char __pBase64Alphabet[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',   //  0 to  7
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',   //  8 to 15
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',   // 16 to 23
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',   // 24 to 31
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',   // 32 to 39
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',   // 40 to 47
        'w', 'x', 'y', 'z', '0', '1', '2', '3',   // 48 to 55
        '4', '5', '6', '7', '8', '9', '+', '/'    // 56 to 63
};

static unsigned int __pBase64Reverse[256] = {
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
        52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,64,0,1,2,3,4,5,6,7,8,9,
        10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,64,26,27,
        28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64
};


CString CEncryption::Base64Encode(const CString& ssString) {
    CString ssResult;
    if (ssString.GetLength()) {
        int dwSize = BASE64_ENCODE_REQSIZE(ssString.GetLength()+1);
        ssResult.SetLength(dwSize - 1);
        CryptBase64Encode((char *) ssString.GetBuffer(), (char *) ssResult.GetBuffer(), &dwSize, ssString.GetLength());
        if (dwSize)
            ssResult.SetLength(dwSize - 1);
    }
    return ssResult;
}

CString CEncryption::Base64Decode(const CString& ssString) {
    CString ssResult;
    if (ssString.GetLength()) {
        int dwSize = BASE64_DECODE_REQSIZE(ssString.GetLength()+1);
        ssResult.SetLength(dwSize - 1);
        CryptBase64Decode((char *) ssString.GetBuffer(), (char *) ssResult.GetBuffer(), &dwSize, ssString.GetLength());
        if (dwSize)
            ssResult.SetLength(dwSize - 1);
    }
    return ssResult;
}

bool CEncryption::CryptBase64Encode(
    const char * lpszString, 
    char * lpszBuffer, 
    int * dwSize,
    int dwSourceSize) {

    int dwString, dwRequired;
    
    if (lpszString == NULL) {
        * dwSize = 0;
        return false;
    }
    if (dwSourceSize > 0) {
        dwString = dwSourceSize;
    } else {
        dwString = base_strlen(lpszString);
    }

    dwRequired = BASE64_ENCODE_REQSIZE(dwString);
    
    if (dwRequired > (* dwSize)) {
        * dwSize = dwRequired;
        return false;
    }
    
    char * p = lpszBuffer;
    int c;    
    unsigned char * q = (unsigned char*) lpszString;
    for(int i = 0; i < dwSourceSize;){
        c = q[i++];
        c *= 256;
        if(i < dwSourceSize) c+=q[i];
        i++;
        c *= 256;
        if(i < dwSourceSize) c+=q[i];
        i++;
        p[0] = __pBase64Alphabet[(c&0x00fc0000) >> 18];
        p[1] = __pBase64Alphabet[(c&0x0003f000) >> 12];
        p[2] = __pBase64Alphabet[(c&0x00000fc0) >> 6];
        p[3] = __pBase64Alphabet[(c&0x0000003f) >> 0];
        if(i > dwSourceSize) p[3]='=';
        if(i > dwSourceSize + 1) p[2]='=';
        p+=4;
    }
    * p = 0; 
    assert(* dwSize >= (p - lpszBuffer + 1));
    * dwSize = p - lpszBuffer + 1;
    return true;
}

bool CEncryption::CryptBase64Decode(
    const char * lpszString, 
    char * lpszBuffer, 
    int * pcbDecoded,
    int dwSourceSize) {

    int nBytesDecoded;    
    char * pszInBuffer = (char *) lpszString;
    unsigned char * pszOutBuffer;    
    
    pszInBuffer = (char *) lpszString;
    while(__pBase64Reverse[*(pszInBuffer++)] <= 63);
    dwSourceSize = (int) (pszInBuffer - lpszString - 1); 

	nBytesDecoded = BASE64_DECODE_REQSIZE(dwSourceSize) - 1;
    
    if (* pcbDecoded < nBytesDecoded) {        
        * pcbDecoded = nBytesDecoded;
        return false;
    }
        
    pszOutBuffer = (unsigned char *) lpszBuffer;

    pszInBuffer = (char *) lpszString;

    while (dwSourceSize > 0) {

        *(pszOutBuffer++) = (unsigned char) (__pBase64Reverse[*pszInBuffer] << 2 | __pBase64Reverse[pszInBuffer[1]] >> 4);
        *(pszOutBuffer++) = (unsigned char) (__pBase64Reverse[pszInBuffer[1]] << 4 | __pBase64Reverse[pszInBuffer[2]] >> 2);
        *(pszOutBuffer++) = (unsigned char) (__pBase64Reverse[pszInBuffer[2]] << 6 | __pBase64Reverse[pszInBuffer[3]]);
        pszInBuffer += 4;
        if (dwSourceSize >= 4) {
            dwSourceSize -= 4;
        } else {
            dwSourceSize = 3;
            break;
        }
    }

    if(dwSourceSize & 03) {
        if(__pBase64Reverse[pszInBuffer[-2]] > 63) {
            nBytesDecoded -= 2;
        } else {
            nBytesDecoded -= 1;
        }
    }


    lpszBuffer[nBytesDecoded++] = '\0';

    * pcbDecoded = nBytesDecoded;

    return true;
}


