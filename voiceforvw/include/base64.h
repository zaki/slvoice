/* base64.h
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);
