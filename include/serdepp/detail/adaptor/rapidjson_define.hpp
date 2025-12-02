#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_RAPID_JSON_DEFINE_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_RAPID_JSON_DEFINE_HPP__

#define RAPIDJSON_HAS_STDSTRING 1
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

namespace serde {

std::string to_string(rapidjson::Document &doc);

}

namespace serde {

template<> struct serde_type_checker<rapidjson::Document> {
    using Format = rapidjson::Document;
    static bool is_integer(Format& format);
    static bool is_sequence(Format& format);
    static bool is_map(Format& format);
    static bool is_float(Format& format);
    static bool is_string(Format& format);
    static bool is_bool(Format& format);
    static bool is_null(Format& format);
    static bool is_struct(Format& format);
};

template<> struct serde_type_checker<rapidjson::Value> {
    using Format = rapidjson::Value;
    static bool is_integer(Format& format);
    static bool is_sequence(Format& format);
    static bool is_map(Format& format);
    static bool is_float(Format& format);
    static bool is_string(Format& format);
    static bool is_bool(Format& format);
    static bool is_null(Format& format);
    static bool is_struct(Format& format);
};

template<> struct serde_adaptor_helper<rapidjson::Document> : derive_serde_adaptor_helper<rapidjson::Document> {
    using rapidjson_type = rapidjson::Document;
    static rapidjson_type parse_file(const std::string& path);
    static bool is_null(rapidjson_type& adaptor, std::string_view key);
    static size_t size(rapidjson_type& adaptor);
    static bool is_struct(rapidjson_type& adaptor);
};

template<> 
struct serde_adaptor_helper<rapidjson::Value> : derive_serde_adaptor_helper<rapidjson::Value> {
    static bool is_null(rapidjson::Value& adaptor, std::string_view key);
    static size_t size(rapidjson::Value& adaptor);
    static bool is_struct(rapidjson::Value& adaptor);
};

template<typename T>
struct serde_adaptor<rapidjson::Document, T>  {
    using rapidjson_type = rapidjson::Document;
    constexpr static void from(rapidjson_type& s, std::string_view key, T& data);

    static void into(rapidjson_type& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<rapidjson::Value, T> {
    static void from(rapidjson::Value& s, std::string_view key, T& data);
};

template<>
struct serde_adaptor<rapidjson::Value, std::string>  {
    static void from(rapidjson::Value& s, std::string_view key, std::string& data);
};


template<>
struct serde_adaptor<rapidjson::Document, std::string>  {
    using rapidjson_type = rapidjson::Document;
    static void from(rapidjson_type& s, std::string_view key, std::string& data);
    static void into(rapidjson_type& s, std::string_view key, const std::string& data);
};

template<typename... T>
struct serde_adaptor<rapidjson::Document, std::variant<T...>>  {
    using rapidjson_type = rapidjson::Document;
    constexpr static void from(rapidjson_type& s, std::string_view key, std::variant<T...>& data);
    constexpr static void into(rapidjson_type& s, std::string_view key, const std::variant<T...>& data);
};

template<typename T>
struct serde_adaptor<rapidjson::Document, T, detail::struct_t> {
    using rapidjson_type = rapidjson::Document;
    static void from(rapidjson_type& s, std::string_view key, T& data);
    static void into(rapidjson_type& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<rapidjson::Value, T, detail::struct_t> {
    static void from(rapidjson::Value& s, std::string_view key, T& data);
};

template<typename T>
struct serde_adaptor<rapidjson::Document, T, detail::seq_t> {
    using rapidjson_type = rapidjson::Document;
    using E = detail::seq_e<T>;
    static void from(rapidjson_type& s, std::string_view key, T& arr);
    static void into(rapidjson_type& s, std::string_view key, const T& data);
};

template<typename T>
struct serde_adaptor<rapidjson::Value, T, detail::seq_t> {
    using E = detail::seq_e<T>;
    static void from(rapidjson::Value& s, std::string_view key, T& arr);
};

template <typename Map>
struct serde_adaptor<rapidjson::Document, Map, detail::map_t> {
    using rapidjson_type = rapidjson::Document;
    using E = detail::map_e<Map>;
    static void from(rapidjson_type& s, std::string_view key, Map& map);
    static void into(rapidjson_type& s, std::string_view key, const Map& data);
};


template <typename Map>
struct serde_adaptor<rapidjson::Value, Map, detail::map_t> {
    using E = detail::map_e<Map>;
    static void from(rapidjson::Value& s, std::string_view key, Map& map);
};

}

#endif
