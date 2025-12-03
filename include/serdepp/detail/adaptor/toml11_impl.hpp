#pragma once

#ifndef __SERDEPP_ADAOTOR_DETAIL_TOML11_IMPL_HPP__
#define __SERDEPP_ADAOTOR_DETAIL_TOML11_IMPL_HPP__

namespace serde {

std::string to_string(toml::value& doc) {
    return toml::format(doc);
}

// --------------- serde_type_checker<toml::value> ---------------
bool serde_type_checker<toml::value>::is_integer(Format& format) { return format.is_integer(); }
bool serde_type_checker<toml::value>::is_sequence(Format& format) { return format.is_array(); }
bool serde_type_checker<toml::value>::is_map(Format& format) { return format.is_table(); }
bool serde_type_checker<toml::value>::is_float(Format& format) { return format.is_floating(); }
bool serde_type_checker<toml::value>::is_string(Format& format) { return format.is_string(); }
bool serde_type_checker<toml::value>::is_bool(Format& format) { return format.is_boolean(); }
bool serde_type_checker<toml::value>::is_null(Format& format) { return format.is_empty(); }
bool serde_type_checker<toml::value>::is_struct(Format& format) { return format.is_table(); }

// --------------- serde_type_checker<toml::value> ---------------
bool serde_adaptor_helper<toml::value>::is_null(toml_v& adaptor, std::string_view key) {
    return adaptor.is_table() ? !adaptor.contains(std::string{key}) : true;
}
bool serde_adaptor_helper<toml::value>::is_struct(toml_v& adaptor) { 
    return adaptor.is_table(); 
}
size_t serde_adaptor_helper<toml::value>::size(toml_v& adaptor) { 
    return adaptor.size(); 
}
toml::value serde_adaptor_helper<toml::value>::parse_file(const std::string& path) { 
    return toml::parse(path); 
}

// --------------- struct serde_adaptor<toml::value, T, detail::struct_t> ---------------
template<typename T> 
void serde_adaptor<toml::value, T, detail::struct_t>::from(toml_v& s, std::string_view key, T& data) {
    deserialize_to<T>(toml::find(s, std::string{key}), data);
}
template<typename T> 
void serde_adaptor<toml::value, T, detail::struct_t>::into(toml_v &s, std::string_view key, const T& data) {
    s[std::string{key}] = serialize<toml_v>(data);
} 

// --------------- serde_adaptor<toml::value, T> ---------------
template<typename T> 
constexpr void serde_adaptor<toml::value, T>::from(toml_v& s, std::string_view key, T& data) {
    data = key.empty() ? toml::get<T>(s) : toml::find<T>(s, std::string{key});
}
template<typename T> 
constexpr void serde_adaptor<toml::value, T>::into(toml_v& s, std::string_view key, const T& data) {
    if(key.empty()) { s = data; } else { s[key.data()] = data; }
}

// --------------- serde_adaptor<toml::value, T, detail::seq_t> ---------------
template<typename T> 
void serde_adaptor<toml::value, T, detail::seq_t>::from(toml_v& s, std::string_view key, T& arr) {
    auto& table = key.empty() ? s : s[std::string{key}];
    if constexpr(detail::is_reserveable_v<T>) arr.reserve(table.size());
    for(auto& value : table.as_array()) { arr.push_back(deserialize<E>(value)); }
}
template<typename T> 
void serde_adaptor<toml::value, T, detail::seq_t>::into(toml_v& s, std::string_view key, const T& data) {
    toml::array arr;
    arr.reserve(data.size());
    if constexpr(detail::is_struct_v<E>) {
        for(auto& value: data) { arr.push_back(std::move(serialize<toml_v>(value).as_table())); }
    } else {
        for(auto& value: data) { arr.push_back(std::move(serialize<toml_v>(value)));  }
    }
    (key.empty() ? s : s[std::string{key}]) = std::move(arr);
}

// --------------- serde_adaptor<toml::value, Map, detail::map_t> ---------------
template <typename Map> 
void serde_adaptor<toml::value, Map, detail::map_t>::from(toml_v& s, std::string_view key, Map& map) {
    auto& table = key.empty() ? s : s[std::string{key}];
    for(auto& [key_, value_] : table.as_table()) { deserialize_to<E>(value_, map[key_]);}
}
template <typename Map> 
void serde_adaptor<toml::value, Map, detail::map_t>::into(toml_v& s, std::string_view key, const Map& data) {
    toml_v& map = key.empty() ? s : s[std::string{key}];
    for(auto& [key_, value] : data) { serialize_to<toml_v>(value, map[key_]); }
}

}

#endif
