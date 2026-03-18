#include "worker_engine.h"
#include "json.hpp"
#include <sstream>
#include <iomanip>

std::string format_payload(const char* data, size_t len) {
    try {
        auto j = nlohmann::json::parse(data, data + len);
        
        std::string venue = j.contains("Venue") ? j["Venue"].get<std::string>() : (j.contains("venue") ? j["venue"].get<std::string>() : "UNKNOWN");
        std::string symbol = j.contains("Symbol") ? j["Symbol"].get<std::string>() : (j.contains("symbol") ? j["symbol"].get<std::string>() : "UNKNOWN");
        
        std::string price_str = "0.00";
        if (j.contains("Price") && j["Price"].is_number()) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << j["Price"].get<double>();
            price_str = ss.str();
        } else if (j.contains("price") && j["price"].is_number()) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << j["price"].get<double>();
            price_str = ss.str();
        } else if (j.contains("Price") && j["Price"].is_string()) {
            price_str = j["Price"].get<std::string>();
        }
        
        std::string size_str = "0";
        if (j.contains("Size") && j["Size"].is_number()) {
            size_str = std::to_string(j["Size"].get<int>());
        } else if (j.contains("size") && j["size"].is_number()) {
            size_str = std::to_string(j["size"].get<int>());
        }

        std::ostringstream out;
        out << "**Venue:**" << venue 
            << "|**Symbol:**" << symbol 
            << "|**Price:**" << price_str 
            << "|**Size:**" << size_str;
        return out.str();
    } catch (...) {
        return "**Error:**InvalidJSON";
    }
}
