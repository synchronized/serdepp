#pragma once

#include <serdepp/serde.hpp>
#include <serdepp/adaptor/toml11.hpp>

#include <serdepp/extend/rttr.hpp>

#include <rttr/type>

namespace serde {
    using toml_v = toml::value;

    template <>
    struct serde_adaptor<toml_v, rttr::variant, type::basic_t> {
        using ValueType = rttr::variant;
        inline static void from(toml_v& s, std::string_view key, ValueType& data) {
            ValueType val;
            auto& avalue = key.empty() ? s : s[key.data()];
            if (avalue.is_empty()) {
                return;
            } else if (avalue.is_boolean()) {
                val = (bool)avalue.as_boolean();
            } else if (avalue.is_integer()) {
                val = (int64_t)avalue.as_integer();
            } else if (avalue.is_floating()) {
                val = (double)avalue.as_floating();
            } else if (avalue.is_string()) {
                std::string str_val = avalue.as_string();
                val = str_val;
            } else if (avalue.is_local_date()) {
                using namespace std::chrono;
                system_clock::time_point tpoint = avalue.as_local_date();
                val = tpoint;
            } else if (avalue.is_local_datetime()) {
                using namespace std::chrono;
                system_clock::time_point tpoint = avalue.as_local_datetime();
                val = tpoint;
            } else if (avalue.is_local_time()) {
                using namespace std::chrono;
                nanoseconds tpoint = avalue.as_local_time();
                val = tpoint;
            } else if (avalue.is_offset_datetime()) {
                using namespace std::chrono;
                system_clock::time_point tpoint = avalue.as_offset_datetime();
                val = tpoint;
            }
            if (val) {
                if (!data || val.convert(data.get_type())) {
                    data = val;
                }
            }
        }
        inline static void into(toml_v& s, std::string_view key, const ValueType& data) {
            auto data_type = data.get_type();
            auto wrapped_type = data_type.is_wrapper() 
                    ? data_type.get_wrapped_type() : data_type;

            auto& avalue = key.empty() ? s : s[key.data()];
            if (wrapped_type.is_arithmetic()) {
                if (wrapped_type == rttr::type::get<bool>()) {
                    avalue = data.to_bool();
                    return;
                } else if (wrapped_type == rttr::type::get<char>() 
                            || wrapped_type == rttr::type::get<int8_t>() 
                            || wrapped_type == rttr::type::get<int16_t>() 
                            || wrapped_type == rttr::type::get<int32_t>()) 
                {
                    avalue = data.to_int32();
                    return;
                } else if (wrapped_type == rttr::type::get<int64_t>()) {
                    avalue = data.to_int64();
                    return;
                } else if (wrapped_type == rttr::type::get<uint8_t>() 
                            || wrapped_type == rttr::type::get<uint16_t>() 
                            || wrapped_type == rttr::type::get<uint32_t>()) 
                {
                    avalue = data.to_uint32();
                    return;
                } else if (wrapped_type == rttr::type::get<uint64_t>()) {
                    avalue = data.to_uint64();
                    return;
                } else if (wrapped_type == rttr::type::get<float>() 
                            || wrapped_type == rttr::type::get<double>()) 
                {
                    avalue = data.to_double();
                    return;
                }
            } 

            else if (wrapped_type.is_enumeration()) {
                auto enum_type = wrapped_type.get_enumeration();
                bool ok;
                int int_value = data.to_int(&ok);
                if (ok) {
                    avalue = int_value;
                    return;
                }
                std::string str_value = data.to_string(&ok);
                if (ok) {
                    avalue = str_value;
                    return;
                }
            } 

            else if (wrapped_type == rttr::type::get<std::string>()) {
                avalue = data.to_string();
                return;
            } 

            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                avalue = str_value; 
                return;
            }
        }
    };

    template <>
    struct serde_adaptor<toml_v, rttr::variant_polymoph_view, type::poly_t> {
        using type_checker = serde_type_checker<toml_v>;
        using Container = rttr::variant_polymoph_view;
        using E = rttr::variant;
        inline static void from(toml_v& s, std::string_view key, Container& container) {
            auto& _obj = key.empty() ? s : s.at(std::string{key});
            if (_obj.is_empty() || !_obj.is_table()) return;

            auto obj = _obj.as_table();
            auto a_type_name = obj["$typeName"];
            if (a_type_name.is_empty() || !a_type_name.is_string()) return;
            std::string type_name = a_type_name.as_string();

            auto a_value = obj["$content"];
            if (a_value.is_empty() || !a_value.is_table()) return;
            if (!container.create(type_name)) return;
            rttr::variant value = container.get_value();
            deserialize_to(a_value, value);
        }
        inline static void into(toml_v& s, std::string_view key, const Container& data) {
            if (!data.is_valid()) return;

            std::string type_name = data.get_type_name();
            rttr::variant value = data.get_value();

            auto map = toml::table{
                {"$typeName" , std::move(serialize<toml_v>(type_name))},
                {"$content", std::move(serialize<toml_v>(value))},
            };
            (key.empty() ? s : s[std::string{key}]) = std::move(map);
        }
    };

    template <>
    struct serde_adaptor<toml_v, rttr::variant_sequential_view, type::seq_t> {
        using type_checker = serde_type_checker<toml_v>;
        using Sequent = rttr::variant_sequential_view;
        using E = rttr::variant;
        inline static void from(toml_v& s, std::string_view key, Sequent& seq) {
            auto& arr = key.empty() ? s : s.at(std::string{key});
            seq.set_size(arr.size());
            const rttr::type orig_value_type = seq.get_value_type();
            for (size_t i=0; i<arr.size(); i++) {
                auto& jvalue = arr[i];

                E orig_value = seq.get_value(i);
                rttr::type value_type1 = orig_value.get_type();
                E value = orig_value.get_type().is_wrapper() 
                        ? orig_value.extract_wrapped_value() : orig_value;
                rttr::type value_type = value.get_type();

                deserialize_to(jvalue, value);

                seq.set_value(i, value);
            }
        }
        inline static void into(toml_v& s, std::string_view key, const Sequent& data) {
            toml::array arr;
            arr.reserve(data.get_size());
            for(auto& value : data) { 
                arr.push_back(std::move(serialize<toml_v>(value))); 
            }
            (key.empty() ? s : s[std::string{key}]) = std::move(arr);
        }
    };

    template <>
    struct serde_adaptor<toml_v, rttr::variant_associative_view, type::map_t> {
        using type_checker = serde_type_checker<toml_v>;
        using Map = rttr::variant_associative_view;
        using E = rttr::variant;
        inline static void from(toml_v& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s.at(std::string{key});
            if (map.is_key_only_type()) {
                const rttr::type orig_key_type = map.get_key_type();
                if (table.is_array()) {
                    if (serde_rttr_type_checker::is_basic(orig_key_type)) {
                        for (auto& jvalue : table.as_array()) {
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
                        for (auto& jvalue : table.as_array()) {
                            E orig_key = orig_key_type.create();
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_value() : orig_key;
                            deserialize_to(jvalue, key);
                            if (key.get_type() != orig_key_type) {
                                if (!key.convert(orig_key_type)) {
                                    continue;
                                }
                            }
                            map.insert(key);
                        }
                    }
                }
            } else {
                const rttr::type orig_key_type = map.get_key_type();
                const rttr::type orig_value_type = map.get_value_type();
                if (serde_rttr_type_checker::is_basic(orig_key_type)) {
                    if (table.is_table()) {
                        if (serde_rttr_type_checker::is_basic(orig_value_type)) {
                            for(auto& [jkey, jvalue] : table.as_table()) { 
                                std::string str_key = jkey;
                                E key = str_key;

                                E value;
                                deserialize_to(jvalue, value);
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
                            for(auto& [jkey, jvalue] : table.as_table()) { 
                                std::string str_key = jkey;
                                E key = str_key;

                                E orig_value = orig_value_type.create();
                                rttr::type value_type1 = orig_value.get_type();
                                E value = orig_value.get_type().is_wrapper() 
                                        ? orig_value.extract_wrapped_value() : orig_value;
                                rttr::type value_type = value.get_type();
                                deserialize_to(jvalue, value);
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
                else {
                    if (table.is_array()) {
                        if (serde_rttr_type_checker::is_basic(orig_value_type)) {
                            for(auto& _jvalue : table.as_array()) { 
                                if (_jvalue.is_table()) {
                                    auto& jvalue = _jvalue.as_table();
                                    E orig_key = orig_key_type.create(); 
                                    E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                            ? orig_key.extract_wrapped_value() : orig_key;
                                    E orig_value = orig_value_type.create();
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
                            }
                        } else {
                            for(auto& _jvalue : table.as_array()) { 
                                if (_jvalue.is_table()) {
                                    auto& jvalue = _jvalue.as_table();
                                    E orig_key = orig_key_type.create(); 
                                    E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                            ? orig_key.extract_wrapped_value() : orig_key;
                                    E orig_value = orig_value_type.create();
                                    E value = orig_value.get_type().get_raw_type().is_wrapper() 
                                            ? orig_value.extract_wrapped_value() : orig_value;
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
            }
        }
        inline static void into(toml_v& s, std::string_view key, const Map& data) {
            toml_v map;
            if (data.is_key_only_type()) {
                map = toml::array();
                auto& real_arr = map.as_array();
                real_arr.reserve(data.get_size());
                for(auto& [key_, _] : data) { 
                    map.push_back(std::move(serialize<toml_v>(key_))); 
                }
            } else {
                auto orig_key_type = data.get_key_type();
                if (serde_rttr_type_checker::is_basic(orig_key_type)) {
                    map = toml::table{};
                    auto& real_map = map.as_table();
                    real_map.reserve(data.get_size());
                    for(auto& [key_, value_] : data) { 
                        bool ok = false;
                        auto str_key = key_.to_string(&ok);
                        if (ok) {
                            serialize_to<toml_v>(value_, map[str_key]); 
                            continue;
                        }
                        auto num_key = key_.to_int(&ok);
                        if (ok) {
                            serialize_to<toml_v>(value_, map[num_key]); 
                            continue;
                        }
                    }
                } 
                else {
                    map = toml::array();
                    auto& real_arr = map.as_array();
                    real_arr.reserve(data.get_size());
                    for(auto& [key_, value_] : data) { 
                        toml::table obj_value = {
                            {"key" , std::move(serialize<toml_v>(key_))},
                            {"value", std::move(serialize<toml_v>(value_))},
                        };
                        map.push_back(obj_value); 
                    }
                }
            }
            (key.empty() ? s : s[std::string{key}]) = std::move(map);
        }
    };

    template<>
    struct serde_adaptor<toml_v, rttr::variant, type::struct_t> {
        using Object = rttr::variant;
        static void from(toml_v& s, std::string_view key, Object& orig_data) {
            rttr::type orig_type = orig_data.get_type();
            rttr::instance data(orig_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_type();

            auto& table = key.empty() ? s : s.at(std::string{key});
            for(auto& [key_, value_] : table.as_table()) { 
                rttr::property prop = data_type.get_property(key_);
                if (prop) {
                    rttr::variant prop_val = prop.get_value(obj);
                    deserialize_to<rttr::variant>(value_, prop_val); 
                    prop.set_value(obj, prop_val);
                }
            }
        }
        static void into(toml_v& s, std::string_view key, const Object& _data) {
            rttr::instance data(_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_derived_type();

            toml::table map;
            for(auto& prop : data_type.get_properties()) {
                const rttr::variant prop_value = prop.get_value(obj);
                auto prop_name = prop.get_name().to_string();
                serialize_to<toml_v>(prop_value, map[prop_name]);
            }
            (key.empty() ? s : s[std::string{key}]) = std::move(map);
        } 
    };

}