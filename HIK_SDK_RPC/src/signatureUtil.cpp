#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include "signatureUtil.h"

std::string base64_encode(const char *in_str, int in_len)
{
    BIO *b64, *bio;
    BUF_MEM *bptr = NULL;
    size_t size = 0;

    if (in_str == NULL)
        return "";

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio, in_str, in_len);
    (void)BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bptr);
    std::string result;
    size = bptr->length;
	result.append(bptr->data, size); 
    BIO_free_all(bio);
    return result;
}

std::string base64_decode(const char *input, int length)
{
    BIO * b64 = NULL;
    BIO * bmem = NULL;
    char buffer[4096] = {0};
 
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new_mem_buf((void *)input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);
 
    BIO_free_all(bmem);
    std::string result = buffer;
    return result;
}

unsigned char ToHex(unsigned char x) 
{ 
    return  x > 9 ? x + 55 : x + 48; 
}
 
unsigned char FromHex(unsigned char x) 
{ 
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}
 
std::string UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}
 
std::string UrlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}

std::string getMD5(const std::string& src )
{
    MD5_CTX ctx;
 
    std::string md5_string;
    unsigned char md[16] = { 0 };
    char tmp[33] = { 0 };
 
    MD5_Init( &ctx );
    MD5_Update( &ctx, src.c_str(), src.size() );
    MD5_Final( md, &ctx );
 
    for( int i = 0; i < 16; ++i ) {   
        memset( tmp, 0x00, sizeof( tmp ) );
        sprintf( tmp, "%02X", md[i] );
        md5_string += tmp;
    }   
    return md5_string;
}
