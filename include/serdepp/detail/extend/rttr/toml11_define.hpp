#pragma once

#include <rttr/type>

namespace serde {

template <>
struct serde_adaptor<toml::value, rttr::variant, detail::basic_t> {
    using toml_v = toml::value;
    using ValueType = rttr::variant;
    static void from(toml_v& s, std::string_view key, ValueType& data);
    static void into(toml_v& s, std::string_view key, const ValueType& orig_data);
};

template <>
struct serde_adaptor<toml::value, rttr::variant_polymoph_view, detail::poly_t> {
    using toml_v = toml::value;
    using type_checker = serde_type_checker<toml_v>;
    using Container = rttr::variant_polymoph_view;
    using E = rttr::variant;
    static void from(toml_v& s, std::string_view key, Container& container);
    static void into(toml_v& s, std::string_view key, const Container& data);
};

template <>
struct serde_adaptor<toml::value, rttr::variant_sequential_view, detail::seq_t> {
    using toml_v = toml::value;
    using type_checker = serde_type_checker<toml_v>;
    using Sequent = rttr::variant_sequential_view;
    using E = rttr::variant;
    static void from(toml_v& s, std::string_view key, Sequent& seq);
    static void into(toml_v& s, std::string_view key, const Sequent& data);
};

template <>
struct serde_adaptor<toml::value, rttr::variant_associative_view, detail::map_t> {
    using toml_v = toml::value;
    using type_checker = serde_type_checker<toml_v>;
    using Map = rttr::variant_associative_view;
    using E = rttr::variant;
    static void from(toml_v& s, std::string_view key, Map& map);
    static void into(toml_v& s, std::string_view key, const Map& data);
};

template<>
struct serde_adaptor<toml::value, rttr::variant, detail::struct_t> {
    using toml_v = toml::value;
    using Object = rttr::variant;
    static void from(toml_v& s, std::string_view key, Object& orig_data);
    static void into(toml_v& s, std::string_view key, const Object& _data);
};

}