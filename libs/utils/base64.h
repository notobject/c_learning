
#ifndef XYSSL_BASE64_H
#define XYSSL_BASE64_H

#define XYSSL_ERR_BASE64_BUFFER_TOO_SMALL               -0x0010
#define XYSSL_ERR_BASE64_INVALID_CHARACTER              -0x0012

int base64_encode( unsigned char *dst, int *dlen, unsigned char *src, int  slen );
int base64_decode( unsigned char *dst, int *dlen, unsigned char *src, int  slen );

#endif
