#pragma once

#include <rttr/type>

namespace serde {

template <>
struct serde_adaptor<rapidjson::Document, rttr::variant, detail::basic_t> {
    using rapidjson_type = rapidjson::Document;
    using ValueType = rttr::variant;
    static void from(rapidjson_type& s, std::string_view key, ValueType& data);
    static void into(rapidjson_type& s, std::string_view key, const ValueType& orig_data);
};

template <>
struct serde_adaptor<rapidjson::Document, rttr::variant_polymoph_view, detail::poly_t> {
    using rapidjson_type = rapidjson::Document;
    using Container = rttr::variant_polymoph_view;
    using E = rttr::variant;
    static void from(rapidjson_type& s, std::string_view key, Container& container);
    static void into(rapidjson_type& s, std::string_view key, const Container& data);
};

template <>
struct serde_adaptor<rapidjson::Document, rttr::variant_sequential_view, detail::seq_t> {
    using rapidjson_type = rapidjson::Document;
    using Sequent = rttr::variant_sequential_view;
    using E = rttr::variant;
    static void from(rapidjson_type& s, std::string_view key, Sequent& seq);
    static void into(rapidjson_type& s, std::string_view key, const Sequent& data);
};

template <>
struct serde_adaptor<rapidjson::Document, rttr::variant_associative_view, detail::map_t> {
    using rapidjson_type = rapidjson::Document;
    using Map = rttr::variant_associative_view;
    using E = rttr::variant;
    static void from(rapidjson_type& s, std::string_view key, Map& map);
    static void into(rapidjson_type& s, std::string_view key, const Map& data);
};

template<>
struct serde_adaptor<rapidjson::Document, rttr::variant, detail::struct_t> {
    using rapidjson_type = rapidjson::Document;
    using Object = rttr::variant;
    static void from(rapidjson_type& s, std::string_view key, Object& orig_data);
    static void into(rapidjson_type& s, std::string_view key, const Object& _data);
};

}

namespace serde {

template <>
struct serde_adaptor<rapidjson::Value, rttr::variant, detail::basic_t> {
    using rapidjson_value = rapidjson::Value;
    using ValueType = rttr::variant;
    static void from(rapidjson_value& s, std::string_view key, ValueType& data);
    static void into(rapidjson_value& s, std::string_view key, const ValueType& orig_data);
};

template <>
struct serde_adaptor<rapidjson::Value, rttr::variant_polymoph_view, detail::poly_t> {
    using rapidjson_value = rapidjson::Value;
    using Container = rttr::variant_polymoph_view;
    using E = rttr::variant;
    static void from(rapidjson_value& s, std::string_view key, Container& container);
};

template <>
struct serde_adaptor<rapidjson::Value, rttr::variant_sequential_view, detail::seq_t> {
    using rapidjson_value = rapidjson::Value;
    using Sequent = rttr::variant_sequential_view;
    using E = rttr::variant;
    static void from(rapidjson_value& s, std::string_view key, Sequent& seq);
};

template <>
struct serde_adaptor<rapidjson::Value, rttr::variant_associative_view, detail::map_t> {
    using rapidjson_value = rapidjson::Value;
    using Map = rttr::variant_associative_view;
    using E = rttr::variant;
    static void from(rapidjson_value& s, std::string_view key, Map& map);
};

template<>
struct serde_adaptor<rapidjson::Value, rttr::variant, detail::struct_t> {
    using rapidjson_value = rapidjson::Value;
    using Object = rttr::variant;
    static void from(rapidjson_value& s, std::string_view key, Object& orig_data);
};

}