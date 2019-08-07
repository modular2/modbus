#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>
std::string base64_encodeB(unsigned char const* , unsigned int len);
std::string base64_decodeB(std::string const& encoded_string) ;
#endif 
