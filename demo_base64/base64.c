#include <stdio.h>
#include "base64.h"

static const unsigned char base64_enc_map[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const unsigned char base64_dec_map[128] =
{
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
     54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
    127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
      5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 127, 127, 127, 127, 127
};

int base64_encode( unsigned char *dst, int *dlen,
                   unsigned char *src, int  slen )
{
    int i, n;
    int C1, C2, C3;
    unsigned char *p;

    if( slen == 0 )
        return( 0 );

    n = (slen << 3) / 6;

    switch( (slen << 3) - (n * 6) )
    {
        case  2: n += 3; break;
        case  4: n += 2; break;
        default: break;
    }

    if( *dlen < n + 1 )
    {
        *dlen = n + 1;
        return( XYSSL_ERR_BASE64_BUFFER_TOO_SMALL );
    }

    n = (slen / 3) * 3;

    for( i = 0, p = dst; i < n; i += 3 )
    {
        C1 = *src++;
        C2 = *src++;
        C3 = *src++;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 &  3) << 4) + (C2 >> 4)) & 0x3F];
        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
        *p++ = base64_enc_map[C3 & 0x3F];
    }

    if( i < slen )
    {
        C1 = *src++;
        C2 = ((i + 1) < slen) ? *src++ : 0;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

        if( (i + 1) < slen )
             *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
        else *p++ = '=';

        *p++ = '=';
    }

    *dlen = p - dst;
    *p = 0;

    return( 0 );
}

int base64_decode( unsigned char *dst, int *dlen, unsigned char *src, int slen )
{
    int i, n, k = 0, count = 0;
    unsigned int j;
    unsigned char *p; 

    if (src == NULL || dst == NULL || dlen == NULL || *dlen <= 0 || slen <= 3)
    {   
        return( -1);
    }   

    /* Skip spaces before checking for EOL */
    i = slen - 1;
    while( i >= 0 && (src[i] == ' ' || src[i] == '\r' || src[i] == '\n') )
    {   
        --i;
    }   
    /* Spaces at end of buffer are OK */
    if( 0 == i ) 
    {   
        *dlen = 0;
        return 0;
    }   

    slen = i + 1;

    unsigned char tmp[4];
    for( j = 3, i = n = 0, p = dst; i < slen; )
    {   
        if( *src == '\r' || *src == '\n' )
        {
            i++, src++;
            continue;
        }

#ifdef _BASE64_CHECKDATA
        /* Space inside a line is an error */
        if(( *src == ' ' ) ||
            ( *src == '=' && ++k > 2 ) ||
            ( *src > 127 || base64_dec_map[*src] == 127 ) ||
            ( base64_dec_map[*src] < 64 && k != 0 ))
        {
            return( XYSSL_ERR_BASE64_INVALID_CHARACTER );
        }

        tmp[n] = base64_dec_map[*src];
        i++, src++;
#else
        tmp[0] = base64_dec_map[src[0]];
        tmp[1] = base64_dec_map[src[1]];
        tmp[2] = base64_dec_map[src[2]];
        tmp[3] = base64_dec_map[src[3]];
        i += 4, src += 4;
#endif

#ifdef _BASE64_CHECKDATA
        if( ++n == 4 )
#endif
        {
            j -= (( tmp[0] == 64 ) + ( tmp[1] == 64 ) + ( tmp[2] == 64 ) + ( tmp[3] == 64 ));
            n = 0;
            count += j;
            if( *dlen < count )
            {
                return( XYSSL_ERR_BASE64_BUFFER_TOO_SMALL );
            }
            if( j > 0 ) *p++ = (unsigned char)( ((tmp[0] & 0x3F) << 2) | ((tmp[1] >> 4) & 0x3F));
            if( j > 1 ) *p++ = (unsigned char)( ((tmp[1] & 0xF) << 4) | ((tmp[2] >> 2) & 0xF) );
            if( j > 2 ) *p++ = (unsigned char)( ((tmp[2] & 0x3) << 6) | (tmp[3] & 0x3F) );
        }
    }

    *dlen = p - dst;

    return( 0 );
}

int base64_decode_old( unsigned char *dst, int *dlen, unsigned char *src, int  slen )
{
    int i, j, n;
    unsigned long x;
    unsigned char *p;

    for( i = j = n = 0; i < slen; i++ )
    {
        if( ( slen - i ) >= 2 &&
            src[i] == '\r' && src[i + 1] == '\n' )
            continue;

        if( src[i] == '\n' )
            continue;

        if( src[i] == '=' && ++j > 2 ){
            //return( XYSSL_ERR_BASE64_INVALID_CHARACTER );
    }

        if( src[i] > 127 || base64_dec_map[src[i]] == 127 ){
           // return( XYSSL_ERR_BASE64_INVALID_CHARACTER );
    }

        if( base64_dec_map[src[i]] < 64 && j != 0 ){
            //return( XYSSL_ERR_BASE64_INVALID_CHARACTER );
    }

        n++;
    }

    if( n == 0 )
        return( 0 );

    n = ((n * 6) + 7) >> 3;

    if( *dlen < n )
    {
        *dlen = n;
        return( XYSSL_ERR_BASE64_BUFFER_TOO_SMALL );
    }

   for( j = 3, n = x = 0, p = dst; i > 0; i--, src++ )
   {
        if( *src == '\r' || *src == '\n' )
            continue;

        j -= ( base64_dec_map[*src] == 64 );
        x  = (x << 6) | ( base64_dec_map[*src] & 0x3F );

        if( ++n == 4 )
        {
            n = 0;
            if( j > 0 ) *p++ = (unsigned char)( x >> 16 );
            if( j > 1 ) *p++ = (unsigned char)( x >>  8 );
            if( j > 2 ) *p++ = (unsigned char)( x       );
        }
    }

    *dlen = p - dst;

    return( 0 );
}
