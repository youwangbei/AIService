//
// Base64编码和解码的C++实现
// 版本: 2.rc.09 (发布候选版)
//

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

#include <string>

#if __cplusplus >= 201703L
#include <string_view>
#endif  // __cplusplus >= 201703L

// Base64编码，可选URL安全模式
std::string base64_encode     (std::string const& s, bool url = false);
// Base64编码，PEM格式
std::string base64_encode_pem (std::string const& s);
// Base64编码，MIME格式
std::string base64_encode_mime(std::string const& s);

// Base64解码，可选去除换行符
std::string base64_decode(std::string const& s, bool remove_linebreaks = false);
// Base64编码，从二进制数据
std::string base64_encode(unsigned char const*, size_t len, bool url = false);

#if __cplusplus >= 201703L
//
// 使用std::string_view的接口，而非const std::string&
// 需要C++17
// 由Yannic Bonenberger提供 (https://github.com/Yannic)
//
// Base64编码，可选URL安全模式
std::string base64_encode     (std::string_view s, bool url = false);
// Base64编码，PEM格式
std::string base64_encode_pem (std::string_view s);
// Base64编码，MIME格式
std::string base64_encode_mime(std::string_view s);

// Base64解码，可选去除换行符
std::string base64_decode(std::string_view s, bool remove_linebreaks = false);
#endif  // __cplusplus >= 201703L

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */
