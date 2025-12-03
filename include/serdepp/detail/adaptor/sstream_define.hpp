#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_SSTREAM_DEFINE_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_SSTREAM_DEFINE_HPP__

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace serde {

class serde_sstream {
public:
    serde_sstream(char begin = '{', char end = '}');
    void set_wrapper(char begin, char end);

    serde_sstream &add(const std::string &data, std::string_view key = "");

    std::string str() const;

private:
    std::vector<std::string> members;
    char begin_, end_;
};

template<>
struct serde_adaptor_helper<serde_sstream> : derive_serde_adaptor_helper<serde_sstream> {
    inline constexpr static bool is_null(serde_sstream &adaptor, std::string_view key);
    inline constexpr static size_t size(serde_sstream &adaptor);
    inline constexpr static bool is_struct(serde_sstream &adaptor);
};

template<typename T> 
struct serde_adaptor<serde_sstream, T> {
    static void from(serde_sstream& s, std::string_view key, T& data);
    static void into(serde_sstream& s, std::string_view key, const T& data);
};

template<typename T> 
struct serde_adaptor<serde_sstream, T, detail::struct_t> {
    static void from(serde_sstream& s, std::string_view key, T& data);
    static void into(serde_sstream &s, std::string_view key, const T& data);
};

template<typename T> 
struct serde_adaptor<serde_sstream, T, detail::seq_t> {
    static void from(serde_sstream& s, std::string_view key, T& arr);
    static void into(serde_sstream& s, std::string_view key, const T& data);
};

template <typename Map> 
struct serde_adaptor<serde_sstream, Map, detail::map_t> {
    using E = detail::map_e<Map>;
    static void from(serde_sstream& s, std::string_view key, Map& map);
    static void into(serde_sstream& s, std::string_view key, const Map& data);
};

template<typename T>
inline std::string to_string(const T& type);

}

#endif
