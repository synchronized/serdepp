#pragma once

#ifndef __SERDE_FORMAT_DETAIL_NLOHMANN_JSON_DEFINE_HPP__
#define __SERDE_FORMAT_DETAIL_NLOHMANN_JSON_DEFINE_HPP__

#define JSON_HAS_CPP_17 // nlohmann_json string_view support macro

#include <fstream>

#include <nlohmann/json.hpp>

namespace serde {
    std::string to_string(nlohmann::json& doc);
}

namespace serde {

template<> 
struct serde_type_checker<nlohmann::json> {
    using Format = nlohmann::json;
    static bool is_integer(Format& format);
    static bool is_sequence(Format& format);
    static bool is_map(Format& format);
    static bool is_float(Format& format);
    static bool is_string(Format& format);
    static bool is_bool(Format& format);
    static bool is_null(Format& format);
    static bool is_struct(Format& format);
};

template<> 
struct serde_adaptor_helper<nlohmann::json> : derive_serde_adaptor_helper<nlohmann::json> {
    using json = nlohmann::json;
    static json parse_file(const std::string& path);
    static bool is_null(json& adaptor, std::string_view key);
    static size_t size(json& adaptor);
    static bool is_struct(json& adaptor);
};

template<typename T>
struct serde_adaptor<nlohmann::json, T>  {
    using json = nlohmann::json;
    constexpr static void from(json& s, std::string_view key, T& data);
    constexpr static void into(json& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<nlohmann::json, T, detail::struct_t> {
    using json = nlohmann::json;
    static void from(json& s, std::string_view key, T& data);
    static void into(json& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<nlohmann::json, T, detail::seq_t> {
    using json = nlohmann::json;
    using E = detail::seq_e<T>;
    static void from(json& s, std::string_view key, T& arr);
    static void into(json& s, std::string_view key, const T& data);
};

template<typename E>
struct serde_adaptor<nlohmann::json, std::vector<E>, detail::seq_t> {
    using json = nlohmann::json;
    static void from(json& s, std::string_view key, std::vector<E>& arr);
    static void into(json& s, std::string_view key, const std::vector<E>& data);
};

template <typename Map>
struct serde_adaptor<nlohmann::json, Map, detail::map_t> {
    using json = nlohmann::json;
    using E = detail::map_e<Map>;
    static void from(json& s, std::string_view key, Map& map);
    static void into(json& s, std::string_view key, const Map& data);
};

template <typename K, typename E>
struct serde_adaptor<nlohmann::json, std::map<K,E>, detail::map_t> {
    using json = nlohmann::json;
    using Map = std::map<K,E>;
    static void from(json& s, std::string_view key, Map& map);
    static void into(json& s, std::string_view key, const Map& data);
};

}

#endif
