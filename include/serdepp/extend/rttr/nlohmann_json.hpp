#pragma once

#include <nlohmann/json.hpp>
#include <rttr/type>

#include "serdepp/adaptor/nlohmann_json.hpp"
#include "serdepp/detail/rttr.hpp"

namespace serde {
    using nlohmann::json;

    template <>
    struct serde_adaptor<json, rttr::variant, detail::basic_t> {
        using type_checker = serde_type_checker<json>;
        using ValueType = rttr::variant;
        inline static void from(json& s, std::string_view key, ValueType& data) {
            ValueType val;
            auto& jvalue = key.empty() ? s : s.at(std::string{key});
            if (jvalue.is_boolean()) {
                using real_type = bool;
                real_type real_val;
                if (jvalue.get_to(real_val)) {
                    val = real_val;
                }
            } 
            else if (jvalue.is_number_unsigned()) {
                using real_type = uint64_t;
                real_type real_val;
                if (jvalue.get_to(real_val)) {
                    val = real_val;
                }
            }
            else if (jvalue.is_number_integer()) {
                using real_type = int64_t;
                real_type real_val;
                if (jvalue.get_to(real_val)) {
                    val = real_val;
                }
            }
            else if (jvalue.is_number_float()) {
                using real_type = double;
                real_type real_val;
                if (jvalue.get_to(real_val)) {
                    val = real_val;
                }
            }
            else if (jvalue.is_string()) {
                using real_type = std::string;
                real_type real_val;
                jvalue.get_to(real_val);
                val = real_val;
            }
            else if (jvalue.is_binary()) {
                using real_type = std::string;
                real_type real_val;
                jvalue.get_to(real_val);
                val = real_val;
            }
            if (val) {
                if (!data || val.convert(data.get_type().get_raw_type())) {
                    data = val;
                }
            }
        }
        inline static void into(json& s, std::string_view key, const ValueType& orig_data) {
            const rttr::type orig_data_type = orig_data.get_type();
            rttr::variant wrapped_val = orig_data_type.is_wrapper() 
                    ? orig_data.extract_wrapped_ref_value() : orig_data;
            const rttr::type wrapped_type = wrapped_val.get_type();
            rttr::variant data = wrapped_type.is_pointer() 
                    ? wrapped_val.extract_pointer_value() : wrapped_val;
            const rttr::type data_type = data.get_type();

            json& jvalue = key.empty() ? s : s[std::string{key}];

            if (data_type.is_arithmetic()) {
                if (data_type == rttr::type::get<bool>()) {
                    jvalue = data.to_bool();
                    return;
                } else if (data_type == rttr::type::get<char>() 
                        || data_type == rttr::type::get<int8_t>() 
                        || data_type == rttr::type::get<int16_t>() 
                        || data_type == rttr::type::get<int32_t>()) 
                {
                    jvalue = data.to_int32();
                    return;
                } else if (data_type == rttr::type::get<int64_t>()) {
                    jvalue = data.to_int64();
                    return;
                } else if (data_type == rttr::type::get<uint8_t>() 
                        || data_type == rttr::type::get<uint16_t>() 
                        || data_type == rttr::type::get<uint32_t>()) 
                {
                    jvalue = data.to_uint32();
                    return;
                } else if (data_type == rttr::type::get<uint64_t>()) {
                    jvalue = data.to_uint64();
                    return;
                } else if (data_type == rttr::type::get<float>() 
                        || data_type == rttr::type::get<double>()) 
                {
                    jvalue = data.to_double();
                    return;
                }
            } 

            else if (data_type.is_enumeration()) {
                auto enum_type = data_type.get_enumeration();
                bool ok;
                int int_value = data.to_int(&ok);
                if (ok) {
                    jvalue = int_value;
                    return;
                }
                std::string str_value = data.to_string(&ok);
                if (ok) {
                    jvalue = str_value;
                    return;
                }
            } 

            else if (data_type == rttr::type::get<std::string>()) {
                jvalue = data.to_string();
                return;
            }

            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                jvalue = str_value;
                return;
            }

        }
    };

    template <>
    struct serde_adaptor<json, rttr::variant_polymoph_view, detail::poly_t> {
        using type_checker = serde_type_checker<json>;
        using Container = rttr::variant_polymoph_view;
        using E = rttr::variant;
        inline static void from(json& s, std::string_view key, Container& container) {
            auto& arr = key.empty() ? s : s.at(std::string{key});
            if (!arr.is_object()) return;

            json jtypename = arr["$typeName"];
            if (!jtypename.is_string()) return;

            json jvalue = arr["$content"];
            if (!jvalue.is_object()) return;

            std::string type_name;
            type_name = arr["$typeName"].get_to(type_name);
            if (!container.create(type_name)) return;

            rttr::variant value = container.get_value();
            deserialize_to(jvalue, value);
        }
        inline static void into(json& s, std::string_view key, const Container& data) {
            if (!data.is_valid()) return;
            json& arr = key.empty() ? s : s[std::string{key}];

            std::string type_name = data.get_real_type().get_name().to_string();
            rttr::variant value = data.get_value();
            arr = json::object_t{
                {"$typeName", serialize<json>(type_name)},
                {"$content", serialize<json>(value)},
            };
        }
    };

    template <>
    struct serde_adaptor<json, rttr::variant_sequential_view, detail::seq_t> {
        using type_checker = serde_type_checker<json>;
        using Sequent = rttr::variant_sequential_view;
        using E = rttr::variant;
        inline static void from(json& s, std::string_view key, Sequent& seq) {
            auto& arr = key.empty() ? s : s.at(std::string{key});
            seq.set_size(arr.size());
            const rttr::type orig_value_type = seq.get_value_type();
            for (size_t i=0; i<arr.size(); i++) {
                auto& jvalue = arr[i];

                E orig_value = seq.get_value(i);
                rttr::type value_type1 = orig_value.get_type();
                E value = orig_value.get_type().is_wrapper() 
                        ? orig_value.extract_wrapped_ptr_value() : orig_value;
                rttr::type value_type = value.get_type();

                deserialize_to(jvalue, value);

                seq.set_value(i, value);
            }
        }
        inline static void into(json& s, std::string_view key, const Sequent& data) {
            json& arr = key.empty() ? s : s[std::string{key}];
            for(auto& value : data) { 
                arr.push_back(serialize<json>(value)); 
            }
        }
    };

    template <>
    struct serde_adaptor<json, rttr::variant_associative_view, detail::map_t> {
        using type_checker = serde_type_checker<json>;
        using Map = rttr::variant_associative_view;
        using E = rttr::variant;
        inline static void from(json& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s.at(std::string{key});
            if (map.is_key_only_type()) {
                const rttr::type orig_key_type = map.get_key_type();
                if (detail::rttr_type_is_basic(orig_key_type)) {
                    for (auto& jvalue : table) {
                        E key;
                        deserialize_to(jvalue, key);
                        if (key.get_type() != orig_key_type) {
                            if (!key.convert(orig_key_type)) {
                                continue;
                            }
                        }
                        map.insert(key);
                    }
                } else {
                    for (auto& jvalue : table) {
                        E orig_key = orig_key_type.create();
                        E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        deserialize_to(jvalue, key);
                        if (key.get_type() != orig_key_type) {
                            if (!key.convert(orig_key_type)) {
                                continue;
                            }
                        }
                        map.insert(key);
                    }
                }
            } else {
                const rttr::type orig_key_type = map.get_key_type();
                const rttr::type orig_value_type = map.get_value_type();
                if (detail::rttr_type_is_basic(orig_key_type)) {
                    if (detail::rttr_type_is_basic(orig_value_type)) {
                        for(auto& [jkey, jvalue] : table.items()) { 
                            E key = jkey;

                            E value;
                            deserialize_to(jvalue, value);
                            rttr::type value_type = value.get_type();
                            if (key.get_type() != orig_key_type) {
                                if (!key.convert(orig_key_type)) {
                                    continue;
                                }
                            }
                            if (value.get_type() != orig_value_type) {
                                if (!value.convert(orig_value_type)) {
                                    continue;
                                }
                            }
                            map.insert(key, value);
                        }
                    } else {
                        for(auto& [jkey, jvalue] : table.items()) { 
                            E key = jkey;
                            rttr::type key_type = key.get_type();

                            E orig_value = orig_value_type.create();
                            rttr::type value_type1 = orig_value.get_type();
                            E value = orig_value.get_type().is_wrapper() 
                                    ? orig_value.extract_wrapped_ptr_value() : orig_value;
                            rttr::type value_type2 = value.get_type();
                            deserialize_to(jvalue, value);
                            rttr::type value_type3 = value.get_type();
                            if (key.get_type() != orig_key_type) {
                                if (!key.convert(orig_key_type)) {
                                    continue;
                                }
                            }
                            if (value.get_type() != orig_value_type) {
                                if (!value.convert(orig_value_type)) {
                                    continue;
                                }
                            }
                            map.insert(key, value);
                        }
                    }
                }
                else {
                    if (detail::rttr_type_is_basic(orig_value_type)) {
                        for(auto& [jkey, jvalue] : table.items()) { 
                            E orig_key = orig_key_type.create(); 
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_ptr_value() : orig_key;
                            auto key_type = key.get_type();

                            E value;
                            deserialize_to(jvalue["key"], key);
                            deserialize_to(jvalue["value"], value);
                            if (key.get_type() != orig_key_type) {
                                if (!key.convert(orig_key_type)) {
                                    continue;
                                }
                            }
                            if (value.get_type() != orig_value_type) {
                                if (!value.convert(orig_value_type)) {
                                    continue;
                                }
                            }
                            map.insert(key, value);
                        }
                    } else {
                        for(auto& [jkey, jvalue] : table.items()) { 
                            E orig_key = orig_key_type.create(); 
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_ptr_value() : orig_key;
                            auto key_type = key.get_type();
                            E orig_value = orig_value_type.create();
                            E value = orig_value.get_type().get_raw_type().is_wrapper() 
                                    ? orig_value.extract_wrapped_ptr_value() : orig_value;
                            deserialize_to(jvalue["key"], key);
                            deserialize_to(jvalue["value"], value);
                            if (key.get_type() != orig_key_type) {
                                if (!key.convert(orig_key_type)) {
                                    continue;
                                }
                            }
                            if (value.get_type() != orig_value_type) {
                                if (!value.convert(orig_value_type)) {
                                    continue;
                                }
                            }
                            map.insert(key, value);
                        }
                    }
                }
            }
        }
        inline static void into(json& s, std::string_view key, const Map& data) {
            json& map = key.empty() ? s : s[std::string{key}];
            if (data.is_key_only_type()) {
                map = json::array_t{};
                for(auto& [key_, _] : data) { 
                    map.push_back(std::move(serialize<json>(key_))); 
                }
            } else {
                auto key_type = data.get_key_type();
                if (detail::rttr_type_is_basic(key_type.get_raw_type())) {
                    map = json::object_t{};
                    for(auto& [key_, value_] : data) { 
                        bool ok = false;
                        rttr::type key_type = key_.get_type();
                        rttr::type value_type = value_.get_type();
                        auto str_key = key_.to_string(&ok);
                        if (!ok) {
                            continue;
                        }
                        serialize_to<json>(value_, map[str_key]); 
                        continue;
                    }
                } else {
                    map = json::array_t{};
                    for(auto& [key_, value_] : data) { 
                        bool ok = false;
                        json obj_value = json::object_t{
                            {"key" , std::move(serialize<json>(key_))},
                            {"value", std::move(serialize<json>(value_))},
                        };
                        map.push_back(obj_value); 
                    }
                }
            }
        }
    };

    template<>
    struct serde_adaptor<json, rttr::variant, detail::struct_t> {
        using Object = rttr::variant;
        static void from(json& s, std::string_view key, Object& orig_data) {
            rttr::type orig_type = orig_data.get_type();
            rttr::instance data(orig_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_type();

            auto& table = key.empty() ? s : s.at(std::string{key});
            for(auto& [key_, value_] : table.items()) { 
                rttr::property prop = data_type.get_property(key_);
                if (prop) {
                    rttr::variant prop_val = prop.get_value(obj);
                    deserialize_to<rttr::variant>(value_, prop_val); 
                    prop.set_value(obj, prop_val);
                }
            }
        }
        static void into(json& s, std::string_view key, const Object& _data) {
            rttr::instance data(_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_derived_type();

            json& table = key.empty() ? s : s[std::string{key}];
            for(auto& prop : data_type.get_properties()) {
                const rttr::variant prop_value = prop.get_value(obj);
                auto prop_name = prop.get_name();
                serialize_to<json>(prop_value, table[std::string{prop_name}]);
            }
        } 
    };
}
