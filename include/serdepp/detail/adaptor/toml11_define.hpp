#pragma once

#ifndef __SERDEPP_ADAOTOR_DETAIL_TOML11_DEFINE_HPP__
#define __SERDEPP_ADAOTOR_DETAIL_TOML11_DEFINE_HPP__

#include <toml.hpp>

namespace serde {

std::string to_string(toml::value& doc);

template<> 
struct serde_type_checker<toml::value> {
    using Format = toml::value;
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
struct serde_adaptor_helper<toml::value>: derive_serde_adaptor_helper<toml::value> {
    using toml_v = toml::value;
    static bool is_null(toml_v& adaptor, std::string_view key);
    static bool is_struct(toml_v& adaptor);
    static size_t size(toml_v& adaptor);
    static toml_v parse_file(const std::string& path);
};

template<typename T> 
struct serde_adaptor<toml::value, T, detail::struct_t> {
    using toml_v = toml::value;
    static void from(toml_v& s, std::string_view key, T& data);
    static void into(toml_v &s, std::string_view key, const T& data);
};

template<typename T> 
struct serde_adaptor<toml::value, T>  {
    using toml_v = toml::value;
    constexpr static void from(toml_v& s, std::string_view key, T& data);
    constexpr static void into(toml_v& s, std::string_view key, const T& data);
};

template<typename T> 
struct serde_adaptor<toml::value, T, detail::seq_t> {
    using toml_v = toml::value;
    using E = detail::seq_e<T>;
    static void from(toml_v& s, std::string_view key, T& arr);
    static void into(toml_v& s, std::string_view key, const T& data);
};

template <typename Map> 
struct serde_adaptor<toml::value, Map, detail::map_t> {
    using toml_v = toml::value;
    using E = detail::map_e<Map>;
    static void from(toml_v& s, std::string_view key, Map& map);
    static void into(toml_v& s, std::string_view key, const Map& data);
};

}

#endif
