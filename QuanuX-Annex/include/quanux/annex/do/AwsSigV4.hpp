#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace quanux {
namespace annex {
namespace do_impl {

class AwsSigV4 {
public:
    AwsSigV4(const std::string& access_key, const std::string& secret_key, const std::string& region, const std::string& service);

    std::map<std::string, std::string> sign_request(
        const std::string& method,
        const std::string& uri,
        const std::string& payload,
        const std::map<std::string, std::string>& headers,
        const std::map<std::string, std::string>& query_params = {}
    );

private:
    std::string m_access_key;
    std::string m_secret_key;
    std::string m_region;
    std::string m_service;

    std::vector<uint8_t> hmac_sha256(const std::vector<uint8_t>& key, const std::string& msg) const;
    std::string sha256_hex(const std::string& msg) const;
    std::string to_hex(const std::vector<uint8_t>& data) const;
    std::string get_timestamp() const;
    std::string get_date(const std::string& timestamp) const;
};

} // namespace do_impl
} // namespace annex
} // namespace quanux
