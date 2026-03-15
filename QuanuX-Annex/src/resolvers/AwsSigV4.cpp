#include "quanux/annex/do/AwsSigV4.hpp"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iostream>

namespace quanux {
namespace annex {
namespace do_impl {

AwsSigV4::AwsSigV4(const std::string& access_key, const std::string& secret_key, const std::string& region, const std::string& service)
    : m_access_key(access_key), m_secret_key(secret_key), m_region(region), m_service(service) {}

std::vector<uint8_t> AwsSigV4::hmac_sha256(const std::vector<uint8_t>& key, const std::string& msg) const {
    unsigned int len = EVP_MAX_MD_SIZE;
    std::vector<uint8_t> hash(len);
    HMAC(EVP_sha256(), key.data(), key.size(), reinterpret_cast<const unsigned char*>(msg.c_str()), msg.length(), hash.data(), &len);
    hash.resize(len);
    return hash;
}

std::string AwsSigV4::sha256_hex(const std::string& msg) const {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, msg.c_str(), msg.length());
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    EVP_DigestFinal_ex(ctx, hash, &lengthOfHash);
    EVP_MD_CTX_free(ctx);
    return to_hex(std::vector<uint8_t>(hash, hash + lengthOfHash));
}

std::string AwsSigV4::to_hex(const std::vector<uint8_t>& data) const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < data.size(); ++i) {
        ss << std::setw(2) << static_cast<unsigned>(data[i]);
    }
    return ss.str();
}

std::string AwsSigV4::get_timestamp() const {
    std::time_t now = std::time(nullptr);
    char buf[sizeof("YYYYMMDDThhmmssZ")];
    std::strftime(buf, sizeof(buf), "%Y%m%dT%H%M%SZ", std::gmtime(&now));
    return std::string(buf);
}

std::string AwsSigV4::get_date(const std::string& timestamp) const {
    return timestamp.substr(0, 8);
}

std::map<std::string, std::string> AwsSigV4::sign_request(
    const std::string& method,
    const std::string& uri,
    const std::string& payload,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& query_params) {

    std::map<std::string, std::string> signed_headers = headers;
    
    std::string amz_date = get_timestamp();
    std::string date_stamp = get_date(amz_date);
    
    signed_headers["x-amz-date"] = amz_date;
    signed_headers["x-amz-content-sha256"] = "UNSIGNED-PAYLOAD";
    if (signed_headers.find("host") == signed_headers.end()) {
        signed_headers["host"] = m_region + ".digitaloceanspaces.com"; 
    }

    std::string canonical_headers;
    std::string signed_headers_str;
    
    for (auto it = signed_headers.begin(); it != signed_headers.end(); ++it) {
        std::string key = it->first;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower); 
        canonical_headers += key + ":" + it->second + "\n";
        signed_headers_str += key;
        if (std::next(it) != signed_headers.end()) {
            signed_headers_str += ";";
        }
    }

    std::string canonical_query; 
    for(auto it = query_params.begin(); it != query_params.end(); ++it) {
       canonical_query += it->first + "=" + it->second;
       if (std::next(it) != query_params.end()) {
           canonical_query += "&";
       }
    }

    std::string canonical_request = method + "\n" + uri + "\n" + canonical_query + "\n" + canonical_headers + "\n" + signed_headers_str + "\n" + signed_headers["x-amz-content-sha256"];
    
    std::string algorithm = "AWS4-HMAC-SHA256";
    std::string credential_scope = date_stamp + "/" + m_region + "/" + m_service + "/aws4_request";
    std::string string_to_sign = algorithm + "\n" + amz_date + "\n" + credential_scope + "\n" + sha256_hex(canonical_request);

    std::string kSecret_str = "AWS4" + m_secret_key;
    std::vector<uint8_t> kSecret(kSecret_str.begin(), kSecret_str.end());
    std::vector<uint8_t> kDate = hmac_sha256(kSecret, date_stamp);
    std::vector<uint8_t> kRegion = hmac_sha256(kDate, m_region);
    std::vector<uint8_t> kService = hmac_sha256(kRegion, m_service);
    std::vector<uint8_t> kSigning = hmac_sha256(kService, "aws4_request");
    
    std::vector<uint8_t> signature_raw = hmac_sha256(kSigning, string_to_sign);
    std::string signature = to_hex(signature_raw);

    std::string auth_header = algorithm + " Credential=" + m_access_key + "/" + credential_scope + ", SignedHeaders=" + signed_headers_str + ", Signature=" + signature;
    signed_headers["Authorization"] = auth_header;

    return signed_headers;
}

} // namespace do_impl
} // namespace annex
} // namespace quanux
