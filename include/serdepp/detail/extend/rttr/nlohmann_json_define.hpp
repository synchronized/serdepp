#pragma once

#ifndef __SERDEPP_DETAIL_EXTEND_RTTR_NLOHMANN_JSON_DETAIL_HPP__
#define __SERDEPP_DETAIL_EXTEND_RTTR_NLOHMANN_JSON_DETAIL_HPP__

#include <rttr/type>

namespace serde {

template <>
struct serde_adaptor<nlohmann::json, rttr::variant, detail::basic_t> {
    using json = nlohmann::json;
    using ValueType = rttr::variant;
    static void from(json& s, std::string_view key, ValueType& data);
    static void into(json& s, std::string_view key, const ValueType& orig_data);
};

template <>
struct serde_adaptor<nlohmann::json, rttr::variant_polymoph_view, detail::poly_t> {
    using json = nlohmann::json;
    using Container = rttr::variant_polymoph_view;
    using E = rttr::variant;
    static void from(json& s, std::string_view key, Container& container);
    static void into(json& s, std::string_view key, const Container& data);
};

template <>
struct serde_adaptor<nlohmann::json, rttr::variant_sequential_view, detail::seq_t> {
    using json = nlohmann::json;
    using Sequent = rttr::variant_sequential_view;
    using E = rttr::variant;
    static void from(json& s, std::string_view key, Sequent& seq);
    static void into(json& s, std::string_view key, const Sequent& data);
};

template <>
struct serde_adaptor<nlohmann::json, rttr::variant_associative_view, detail::map_t> {
    using json = nlohmann::json;
    using Map = rttr::variant_associative_view;
    using E = rttr::variant;
    static void from(json& s, std::string_view key, Map& map);
    static void into(json& s, std::string_view key, const Map& data);
};

template<>
struct serde_adaptor<nlohmann::json, rttr::variant, detail::struct_t> {
    using json = nlohmann::json;
    using Object = rttr::variant;
    static void from(json& s, std::string_view key, Object& orig_data);
    static void into(json& s, std::string_view key, const Object& _data);
};

}

#endif