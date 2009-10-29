/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/


#ifndef BASE_MD5_HPP
#define BASE_MD5_HPP

#include <platform/include.hpp>
#include <String/String.hpp>

class CMd5 {
	/* Md5 context. */
	typedef struct Md5_CTX {
		unsigned long int state[4]; /* state (ABCD) */
		unsigned long int count[2]; /* number of bits, modulo 2^64 (lsb first) */
		unsigned char buffer[64]; /* input buffer */
	} Md5_CTX;
	/* RC4 key */
	typedef struct rc4_key {
		unsigned char state[256];
		unsigned char x;
		unsigned char y;
	} rc4_key;
private:
	static void Md5Init(Md5_CTX *context);
	static void Md5Update(Md5_CTX * context, const unsigned char * const input, unsigned int inputLen);
	static void Md5Final (unsigned char digest[16], Md5_CTX * context);
	static void Md5Transform (unsigned long int state[4], unsigned char block[64]);
	static void Encode (unsigned char * output, unsigned long int * input, unsigned int len);
	static void Decode (unsigned long int * output, unsigned char * input, unsigned int len);
	static void Rc4PrepareKey(unsigned char *key_data_ptr, int key_data_len, rc4_key *key);
	static inline void swap_byte(unsigned char *a, unsigned char *b);
public:
	CMd5(void);
	virtual ~CMd5(void) = 0;
	static void Md5Calculate(const CString&, CString *);
	static CString Md5(const CString&);
	static void Rc4Calculate(unsigned char *buffer_ptr, int buffer_len, unsigned char * pass_key);
};

inline void CMd5::swap_byte(unsigned char *a, unsigned char *b) {
	unsigned char swapByte;
	swapByte = *a;
	*a = *b;
	*b = swapByte;
}

#endif
