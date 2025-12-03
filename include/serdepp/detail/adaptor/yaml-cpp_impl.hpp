#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_YAML_CPP_IMPL_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_YAML_CPP_IMPL_HPP__

#include <yaml-cpp/yaml.h>

namespace serde {

std::string to_string(YAML::Node& doc) {
    YAML::Emitter emitter;
    // 3. Emit the Node to the Emitter
    emitter << doc;
    return emitter.c_str();
}

// --------------- serde_type_checker<YAML::Node> ---------------
bool serde_type_checker<YAML::Node>::is_integer(Format& format) { return format.IsScalar(); }
bool serde_type_checker<YAML::Node>::is_sequence(Format& format) { return format.IsSequence(); }
bool serde_type_checker<YAML::Node>::is_map(Format& format) { return format.IsMap(); }
bool serde_type_checker<YAML::Node>::is_float(Format& format) { return format.IsMap(); }
bool serde_type_checker<YAML::Node>::is_string(Format& format) { return format.IsScalar(); }
bool serde_type_checker<YAML::Node>::is_bool(Format& format) { return format.IsScalar(); }
bool serde_type_checker<YAML::Node>::is_null(Format& format) { return format.IsNull(); }
bool serde_type_checker<YAML::Node>::is_struct(Format& format) { return format.IsMap(); }

// --------------- serde_adaptor_helper<YAML::Node> ---------------
YAML::Node serde_adaptor_helper<YAML::Node>::parse_file(const std::string& path) { return YAML::LoadFile(path); }
bool serde_adaptor_helper<YAML::Node>::is_null(yaml& adaptor, std::string_view key) { return !adaptor[key.data()]; }
size_t serde_adaptor_helper<YAML::Node>::size(yaml& adaptor)    { return adaptor.size(); }
bool serde_adaptor_helper<YAML::Node>::is_struct(yaml& adaptor) { return adaptor.IsMap(); }

// --------------- serde_adaptor<YAML::Node, T> ---------------
template<typename T>
constexpr void serde_adaptor<YAML::Node, T>::from(yaml& s, std::string_view key, T& data) {
    data = key.empty() ? s.as<T>() : s[key.data()].as<T>();
}
template<typename T>
constexpr void serde_adaptor<YAML::Node, T>::into(yaml& s, std::string_view key, const T& data) {
    if(key.empty()) { s = data; } else { s[key.data()] = data; }
}

// --------------- serde_adaptor<YAML::Node, T, detail::struct_t> ---------------
template<typename T>
void serde_adaptor<YAML::Node, T, detail::struct_t>::from(yaml& s, std::string_view key, T& data) {
    deserialize_to(s[std::string{key}], data);
}
template<typename T>
void serde_adaptor<YAML::Node, T, detail::struct_t>::into(yaml& s, std::string_view key, const T& data) {
    s[std::string{key}] = serialize<yaml>(data);
} 

// --------------- serde_adaptor<YAML::Node, T, detail::seq_t> ---------------
template<typename T>
void serde_adaptor<YAML::Node, T, detail::seq_t>::from(yaml& s, std::string_view key, T& arr) {
    if(key.empty()) {
        if constexpr(detail::is_reserveable_v<T>) arr.reserve(s.size());
        for(std::size_t i = 0 ; i < s.size(); ++i) { arr.push_back(deserialize<E>(s[i])); }
    } else {
        auto table = s[std::string{key}];
        if constexpr(detail::is_reserveable_v<T>) arr.reserve(table.size());
        for(std::size_t i = 0 ; i < table.size(); ++i) { arr.push_back(deserialize<E>(table[i])); }
    }
}
template<typename T>
void serde_adaptor<YAML::Node, T, detail::seq_t>::into(yaml& s, std::string_view key, const T& data) {
    if(key.empty()) {
        for(auto& value: data) { s.push_back(serialize<yaml>(value)); }
    } else {
        for(auto& value: data) { s[std::string{key}].push_back(serialize<yaml>(value)); }
    }
}

// --------------- serde_adaptor<YAML::Node, Map, detail::map_t> ---------------
template <typename Map>
void serde_adaptor<YAML::Node, Map, detail::map_t>::from(yaml& s, std::string_view key, Map& map) {
    if(key.empty()) {
        for(yaml::const_iterator it = s.begin(); it!=s.end(); ++it) {
            auto key_ = it->first, value_ = it->second;
            deserialize_to<E>(value_, map[key_.as<std::string>()]);
        }
    } else {
        auto table = s[std::string{key}];
        for(yaml::const_iterator it = table.begin(); it!=table.end(); ++it) {
            auto key_ = it->first, value_ = it->second;
            deserialize_to<E>(value_, map[key_.as<std::string>()]);
        }
    }
}
template <typename Map>
void serde_adaptor<YAML::Node, Map, detail::map_t>::into(yaml& s, std::string_view key, const Map& data) {
    if(key.empty()) {
        for(auto& [key_, value] : data) {
            s[key_] = serialize<yaml>(value);
        }
    } else {
        yaml map = s[std::string{key}];
        for(auto& [key_, value] : data) {
            map[key_] = serialize<yaml>(value);
        }
    }
}

}

#endif
