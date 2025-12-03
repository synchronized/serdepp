#pragma once

#include <rttr/type>

namespace serde {

template <>
struct serde_adaptor<YAML::Node, rttr::variant, detail::basic_t> {
    using yaml = YAML::Node;
    using ValueType = rttr::variant;
    static void from(yaml& s, std::string_view key, ValueType& data);
    static void into(yaml& s, std::string_view key, const ValueType& orig_data);
};

template <>
struct serde_adaptor<YAML::Node, rttr::variant_polymoph_view, detail::poly_t> {
    using yaml = YAML::Node;
    using type_checker = serde_type_checker<yaml>;
    using Container = rttr::variant_polymoph_view;
    using E = rttr::variant;
    static void from(yaml& s, std::string_view key, Container& container);
    static void into(yaml& s, std::string_view key, const Container& data);
};

template <>
struct serde_adaptor<YAML::Node, rttr::variant_sequential_view, detail::seq_t> {
    using yaml = YAML::Node;
    using type_checker = serde_type_checker<yaml>;
    using Sequent = rttr::variant_sequential_view;
    using E = rttr::variant;
    static void from(yaml& s, std::string_view key, Sequent& seq);
    static void into(yaml& s, std::string_view key, const Sequent& data);
};

template <>
struct serde_adaptor<YAML::Node, rttr::variant_associative_view, detail::map_t> {
    using yaml = YAML::Node;
    using type_checker = serde_type_checker<yaml>;
    using Map = rttr::variant_associative_view;
    using E = rttr::variant;
    static void from(yaml& s, std::string_view key, Map& map);
    static void into(yaml& s, std::string_view key, const Map& data);
};

template<>
struct serde_adaptor<YAML::Node, rttr::variant, detail::struct_t> {
    using yaml = YAML::Node;
    using Object = rttr::variant;
    static void from(yaml& s, std::string_view key, Object& orig_data);
    static void into(yaml& s, std::string_view key, const Object& _data);
};

}