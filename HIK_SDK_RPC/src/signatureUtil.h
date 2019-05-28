#ifndef SIGNATURE_UTIL_H
#define SIGNATURE_UTIL_H
#include <string>
#include <assert.h>

std::string UrlEncode(const std::string& str);
std::string base64_encode(const char *in_str, int in_len);
std::string base64_decode(const char *input, int length);
std::string getMD5(const std::string& src);

#endif
