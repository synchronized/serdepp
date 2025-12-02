#pragma once

#ifndef __SERDE_FORMAT_DETAIL_NLOHMANN_JSON_IMPL_HPP__
#define __SERDE_FORMAT_DETAIL_NLOHMANN_JSON_IMPL_HPP__

namespace serde {
    std::string to_string(nlohmann::json& doc) {
        return doc.dump();
    }
}

namespace serde {

// --------------- serde_type_checker<nlohmann::json> ---------------
bool serde_type_checker<nlohmann::json>::is_integer(Format& format) { return format.is_number(); }
bool serde_type_checker<nlohmann::json>::is_sequence(Format& format) { return format.is_array(); }
bool serde_type_checker<nlohmann::json>::is_map(Format& format) { return format.is_object(); }
bool serde_type_checker<nlohmann::json>::is_float(Format& format) { return format.is_number_float(); }
bool serde_type_checker<nlohmann::json>::is_string(Format& format) { return format.is_string(); }
bool serde_type_checker<nlohmann::json>::is_bool(Format& format) { return format.is_boolean(); }
bool serde_type_checker<nlohmann::json>::is_null(Format& format) { return format.is_null(); }
bool serde_type_checker<nlohmann::json>::is_struct(Format& format) { return format.is_object(); }

// --------------- serde_type_checker<nlohmann::json> ---------------
nlohmann::json serde_adaptor_helper<nlohmann::json>::parse_file(const std::string& path) {
    json table;
    std::ifstream i(path);
    i >> table;
    return table;

}
inline bool serde_adaptor_helper<nlohmann::json>::is_null(nlohmann::json& adaptor, std::string_view key) { 
    return !adaptor.contains(std::string{key}); 
}
inline size_t serde_adaptor_helper<nlohmann::json>::size(nlohmann::json& adaptor) { 
    return adaptor.size(); 
}
inline bool serde_adaptor_helper<nlohmann::json>::is_struct(nlohmann::json& adaptor) { 
    return adaptor.is_object(); 
}

// --------------- serde_adaptor<nlohmann::json, T> ---------------
template<typename T>
constexpr void serde_adaptor<nlohmann::json, T>::from(nlohmann::json& s, std::string_view key, T& data) {
    key.empty() ? s.get_to<T>(data) : s[std::string{key}].get_to<T>(data);
}

template<typename T>
constexpr void serde_adaptor<nlohmann::json, T>::into(nlohmann::json& s, std::string_view key, const T& data) {
    (key.empty() ? s : s[std::string{key}]) = data;
}

// --------------- serde_adaptor<nlohmann::json, std::variant<T...>> ---------------
template<typename... T>
constexpr void serde_adaptor<nlohmann::json, std::variant<T...>>::from(nlohmann::json& s, std::string_view key, std::variant<T...>& data) {
    if(key.empty()) {
        serde_variant_iter<json, std::variant<T...>, T...>(s, data);
    } else {
        serde_variant_iter<json, std::variant<T...>, T...>(s[std::string{key}], data);
    }
}
template<typename... T>
constexpr void serde_adaptor<nlohmann::json, std::variant<T...>>::into(nlohmann::json& s, std::string_view key, const std::variant<T...>& data) {
    std::visit([&](auto& type){ serialize_to<json>(type, s, key); }, data);
}

// --------------- serde_adaptor<nlohmann::json, std::variant<T...>> ---------------
template<typename T>
void serde_adaptor<nlohmann::json, T, detail::struct_t>::from(nlohmann::json& s, std::string_view key, T& data) {
    deserialize_to(s[std::string{key}], data);
}
template<typename T>
void serde_adaptor<nlohmann::json, T, detail::struct_t>::into(nlohmann::json& s, std::string_view key, const T& data) {
    s[std::string{key}] = serialize<nlohmann::json>(data);
} 

// --------------- serde_adaptor<nlohmann::json, T, detail::seq_t> ---------------
template<typename T>
void serde_adaptor<nlohmann::json, T, detail::seq_t>::from(json& s, std::string_view key, T& arr) {
    auto& table = key.empty() ? s : s.at(std::string{key});
    if constexpr(detail::is_reserveable_v<T>) arr.reserve(table.size());
    for(auto& value : table) { arr.push_back(std::move(deserialize<E>(value))); }
}

template<typename T>
void serde_adaptor<nlohmann::json, T, detail::seq_t>::into(json& s, std::string_view key, const T& data) {
    json& arr = key.empty() ? s : s[std::string{key}];
    for(auto& value: data) { arr.push_back(std::move(serialize<json>(value))); }
}

// --------------- serde_adaptor<nlohmann::json, std::vector<E>, detail::seq_t> ---------------
template<typename E>
void serde_adaptor<nlohmann::json, std::vector<E>, detail::seq_t>::from(json& s, std::string_view key, std::vector<E>& arr) {
    auto& table = key.empty() ? s : s.at(std::string{key});
    arr.reserve(table.size());
    for(auto& value : table) { arr.push_back(std::move(deserialize<E>(value))); }
}

template<typename E>
void serde_adaptor<nlohmann::json, std::vector<E>, detail::seq_t>::into(json& s, std::string_view key, const std::vector<E>& data) {
    json& arr = key.empty() ? s : s[std::string{key}];
    for(auto& value: data) { arr.push_back(std::move(serialize<json>(value))); }
}

// --------------- serde_adaptor<nlohmann::json, Map, detail::map_t> ---------------
template <typename Map>
inline void serde_adaptor<nlohmann::json, Map, detail::map_t>::from(json& s, std::string_view key, Map& map) {
    auto& table = key.empty() ? s : s.at(std::string{key});
    for(auto& [key_, value_] : table.items()) { deserialize_to<E>(value_, map[key_]); }
}
template <typename Map>
inline void serde_adaptor<nlohmann::json, Map, detail::map_t>::into(json& s, std::string_view key, const Map& data) {
    json& map = key.empty() ? s : s[std::string{key}];
    for(auto& [key_, value] : data) { serialize_to<json>(value, map[key_]); }
}

// --------------- serde_adaptor<nlohmann::json, std::map<K,E>, detail::map_t> ---------------
template <typename K, typename E>
inline void serde_adaptor<nlohmann::json, std::map<K,E>, detail::map_t>::from(json& s, std::string_view key, Map& map) {
    auto& table = key.empty() ? s : s.at(std::string{key});
    for(auto& [key_, value_] : table.items()) { deserialize_to<E>(value_, map[key_]); }
}

template <typename K, typename E>
inline void serde_adaptor<nlohmann::json, std::map<K,E>, detail::map_t>::into(json& s, std::string_view key, const Map& data) {
    json& map = key.empty() ? s : s[std::string{key}];
    for(auto& [key_, value] : data) { serialize_to<json>(value, map[key_]); }
}

}

#endif
