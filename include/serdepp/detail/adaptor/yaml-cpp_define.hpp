#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_YAML_CPP_DEFINE_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_YAML_CPP_DEFINE_HPP__

#include <yaml-cpp/yaml.h>

namespace serde {

std::string to_string(YAML::Node& doc);

template<> 
struct serde_type_checker<YAML::Node> {
    using Format = YAML::Node;
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
struct serde_adaptor_helper<YAML::Node> : derive_serde_adaptor_helper<YAML::Node> {
    using yaml = YAML::Node;
    static yaml parse_file(const std::string& path);
    static bool is_null(yaml& adaptor, std::string_view key);
    static size_t size(yaml& adaptor);
    static bool is_struct(yaml& adaptor);
};

template<typename T>
struct serde_adaptor<YAML::Node, T>  {
    using yaml = YAML::Node;
    constexpr static void from(yaml& s, std::string_view key, T& data);
    constexpr static void into(yaml& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<YAML::Node, T, detail::struct_t> {
    using yaml = YAML::Node;
    static void from(yaml& s, std::string_view key, T& data);
    static void into(yaml& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<YAML::Node, T, detail::seq_t> {
    using yaml = YAML::Node;
    using E = detail::seq_e<T>;
    static void from(yaml& s, std::string_view key, T& arr);
    static void into(yaml& s, std::string_view key, const T& data);
};


template <typename Map>
struct serde_adaptor<YAML::Node, Map, detail::map_t> {
    using yaml = YAML::Node;
    using E = detail::map_e<Map>;
    static void from(yaml& s, std::string_view key, Map& map);
    static void into(yaml& s, std::string_view key, const Map& data);
};

}

#endif
