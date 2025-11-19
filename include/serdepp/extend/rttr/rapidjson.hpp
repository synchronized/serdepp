#pragma once

#include "serdepp/serializer.hpp"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <fstream>

#include <serdepp/serde.hpp>
#include <serdepp/adaptor/rapidjson.hpp>

#include <serdepp/extend/rttr.hpp>

#include <rttr/type>

namespace serde {
    using rapidjson_type = rapidjson::Document;
    using rapidjson_value = rapidjson::Value;
}

namespace serde {

    template <>
    struct serde_adaptor<rapidjson_value, rttr::variant, detail::basic_t> {
        using ValueType = rttr::variant;
        inline static void from(rapidjson_value& s, std::string_view key, ValueType& data) {
            ValueType val;
            auto& avalue = key.empty() ? s : s[key.data()];
            if (avalue.IsBool()) {
                val = avalue.GetBool();
            } else if (avalue.IsInt()   ) {
                val = avalue.GetInt();
            } else if (avalue.IsUint()  ) {
                val = avalue.GetUint();
            } else if (avalue.IsInt64() ) {
                val = avalue.GetInt64();
            } else if (avalue.IsUint64()) {
                val = avalue.GetUint64();
            } else if (avalue.IsDouble()) {
                val = avalue.GetDouble();
            } else if (avalue.IsString()) {
                std::string strval(avalue.GetString(), avalue.GetStringLength());
                val = strval;
            }
            if (val) {
                if (!data || val.convert(data.get_type().get_raw_type())) {
                    data = val;
                }
            }
        }
        inline static void into(rapidjson_value& s, std::string_view key, const ValueType& orig_data) {
            const rttr::type orig_data_type = orig_data.get_type();
            rttr::variant wrapped_val = orig_data_type.is_wrapper() 
                    ? orig_data.extract_wrapped_ptr_value() : orig_data;
            const rttr::type wrapped_type = wrapped_val.get_type();
            rttr::variant data = wrapped_type.is_pointer() 
                    ? wrapped_val.extract_pointer_value() : wrapped_val;
            const rttr::type data_type = data.get_type();

            auto& avalue = key.empty() ? s : s[key.data()];
            if (data_type.is_arithmetic()) {
                if (data_type == rttr::type::get<bool>()) {
                    avalue.SetBool(data.to_bool());
                    return;
                } else if (data_type == rttr::type::get<char>()) {
                    avalue.SetBool(data.to_bool());
                    return;
                } else if (data_type == rttr::type::get<char>() 
                            || data_type == rttr::type::get<int8_t>() 
                            || data_type == rttr::type::get<int16_t>() 
                            || data_type == rttr::type::get<int32_t>()) 
                {
                    avalue.SetInt(data.to_int32());
                    return;
                } else if (data_type == rttr::type::get<int64_t>()) {
                    avalue.SetInt64(data.to_int64());
                    return;
                } else if (data_type == rttr::type::get<uint8_t>() 
                            || data_type == rttr::type::get<uint16_t>() 
                            || data_type == rttr::type::get<uint32_t>()) 
                {
                    avalue.SetUint(data.to_uint32());
                    return;
                } else if (data_type == rttr::type::get<uint64_t>()) {
                    avalue.SetUint64(data.to_uint64());
                    return;
                } else if (data_type == rttr::type::get<float>() 
                            || data_type == rttr::type::get<double>()) 
                {
                    avalue.SetDouble(data.to_double());
                    return;
                }
            } 

            else if (data_type.is_enumeration()) {
                auto enum_type = data_type.get_enumeration();
                bool ok;
                int int_value = data.to_int(&ok);
                if (ok) {
                    avalue.SetInt(int_value);
                    return;
                }
                std::string str_value = data.to_string(&ok);
                if (ok) {
                    avalue.SetString(str_value.c_str(), str_value.length());
                    return;
                }

            } 

            else if (data_type == rttr::type::get<std::string>()) {
                std::string str_value = data.to_string();
                avalue.SetString(str_value.c_str(), str_value.length());
                return;
            } 

            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                avalue.SetString(str_value.c_str(), str_value.length());
                return;
            }
        }
    };
}

namespace serde {

    template <>
    struct serde_adaptor<rapidjson_type, rttr::variant, detail::basic_t> {
        using ValueType = rttr::variant;
        inline static void from(rapidjson_type& s, std::string_view key, ValueType& data) {
            auto& avalue = key.empty() ? s : s[key.data()];
            ValueType val;
            if (avalue.IsBool()) {
                val = avalue.GetBool();
            } else if (avalue.IsInt()   ) {
                val = avalue.GetInt();
            } else if (avalue.IsUint()  ) {
                val = avalue.GetUint();
            } else if (avalue.IsInt64() ) {
                val = avalue.GetInt64();
            } else if (avalue.IsUint64()) {
                val = avalue.GetUint64();
            } else if (avalue.IsDouble()) {
                val = avalue.GetDouble();
            } else if (avalue.IsString()) {
                std::string strval(avalue.GetString(), avalue.GetStringLength());
                val = strval;
            }
            if (val) {
                if (!data || val.convert(data.get_type().get_raw_type())) {
                    data = val;
                }
            }

        }
        inline static void into(rapidjson_type& s, std::string_view key, const ValueType& orig_data) {
            const rttr::type orig_data_type = orig_data.get_type();
            rttr::variant wrapped_val = orig_data_type.is_wrapper() 
                    ? orig_data.extract_wrapped_ptr_value() : orig_data;
            const rttr::type wrapped_type = wrapped_val.get_type();
            rttr::variant data = wrapped_type.is_pointer() 
                    ? wrapped_val.extract_pointer_value() : wrapped_val;
            const rttr::type data_type = data.get_type();

            auto& alloc = s.GetAllocator();
            auto& avalue = key.empty() ? s : s[key.data()];
            if (data_type.is_arithmetic()) {
                if (data_type == rttr::type::get<bool>()) {
                    avalue.SetBool(data.to_bool());
                    return;
                } else if (data_type == rttr::type::get<char>()) {
                    avalue.SetBool(data.to_bool());
                    return;
                } else if (data_type == rttr::type::get<char>() 
                            || data_type == rttr::type::get<int8_t>() 
                            || data_type == rttr::type::get<int16_t>() 
                            || data_type == rttr::type::get<int32_t>()) 
                {
                    avalue.SetInt(data.to_int32());
                    return;
                } else if (data_type == rttr::type::get<int64_t>()) {
                    avalue.SetInt64(data.to_int64());
                    return;
                } else if (data_type == rttr::type::get<uint8_t>() 
                            || data_type == rttr::type::get<uint16_t>() 
                            || data_type == rttr::type::get<uint32_t>()) 
                {
                    avalue.SetUint(data.to_uint32());
                    return;
                } else if (data_type == rttr::type::get<uint64_t>()) {
                    avalue.SetUint64(data.to_uint64());
                    return;
                } else if (data_type == rttr::type::get<float>() 
                            || data_type == rttr::type::get<double>()) 
                {
                    avalue.SetDouble(data.to_double());
                    return;
                }
            } 

            else if (data_type.is_enumeration()) {
                auto enum_type = data_type.get_enumeration();
                bool ok;
                int int_value = data.to_int(&ok);
                if (ok) {
                    avalue.SetInt(int_value);
                    return;
                }
                std::string str_value = data.to_string(&ok);
                if (ok) {
                    avalue.SetString(str_value.c_str(), str_value.length(), alloc);
                    return;
                }

            } 

            else if (data_type == rttr::type::get<std::string>()) {
                std::string str_value = data.to_string();
                avalue.SetString(str_value.c_str(), str_value.length(), alloc);
                return;
            } 

            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                avalue.SetString(str_value.c_str(), str_value.length(), alloc);
                return;
            }

        }
    };

    template <>
    struct serde_adaptor<rapidjson_type, rttr::variant_polymoph_view, detail::poly_t> {
        using Container = rttr::variant_polymoph_view;
        using E = rttr::variant;
        inline static void from(rapidjson_type& s, std::string_view key, Container& container) {
            auto& table = key.empty() ? s : s[key.data()];
            if(!table.IsObject()) { return; }

            auto& a_type_name = table["$typeName"];
            if (!a_type_name.IsString()) return;
            std::string type_name(a_type_name.GetString(), a_type_name.GetStringLength());
            if (type_name.size() == 0) { return; }

            auto& a_value = table["$content"];
            if (!a_value.IsObject()) { return; }
            if (!container.create(type_name)) { return; }

            rttr::variant value = container.get_value();
            deserialize_to(a_value, value);
        }
        inline static void into(rapidjson_type& s, std::string_view key, const Container& data) {
            using namespace rapidjson;
            auto& alloc = s.GetAllocator();

            Value map;
            map.SetObject();
            {
                std::string type_name = data.get_real_type().get_name().to_string();
                Value v(type_name.c_str(), type_name.length(), alloc);
                map.AddMember("$typeName", v.Move(), alloc);
            }
            {
                rttr::variant value = data.get_value();
                Value v;
                v.CopyFrom(serialize<rapidjson_type>(value), alloc);
                map.AddMember("$content", v.Move(), alloc);
            }
            if(key.empty()) {
                s.CopyFrom(map.Move(), alloc);
            }else {
                if(!s.IsObject()) s.SetObject();
                s.AddMember(rapidjson::StringRef(key.data()), map.Move(), alloc);
            } 
        }
    };

    template <>
    struct serde_adaptor<rapidjson_type, rttr::variant_sequential_view, detail::seq_t> {
        using Sequent = rttr::variant_sequential_view;
        using E = rttr::variant;
        inline static void from(rapidjson_type& s, std::string_view key, Sequent& seq) {
            auto& table = key.empty() ? s : s[key.data()];
            if(!table.IsArray()) { return; }
            seq.set_size(table.Size());
            const rttr::type orig_value_type = seq.get_value_type();
            for (size_t i=0; i<table.Size(); i++) {
                auto& avalue = table[i];

                E orig_value = seq.get_value(i);
                rttr::type value_type1 = orig_value.get_type();
                E value = orig_value.get_type().is_wrapper() 
                        ? orig_value.extract_wrapped_ptr_value() : orig_value;
                rttr::type value_type = value.get_type();

                deserialize_to(avalue, value);

                seq.set_value(i, value);
            }
        }
        inline static void into(rapidjson_type& s, std::string_view key, const Sequent& data) {
            using namespace rapidjson;
            auto& alloc = s.GetAllocator();
            rapidjson::Value arr;
            arr.SetArray();
            arr.Reserve(data.get_size(), alloc);
            for(auto& value : data) { 
                Value v;
                v.CopyFrom(serialize<rapidjson_type>(value), alloc);
                arr.PushBack(v.Move(), alloc);
            }
            if(key.empty()) {
                s.CopyFrom(arr.Move(), alloc);
            }else {
                if(!s.IsObject()) s.SetObject();
                s.AddMember(rapidjson::StringRef(key.data()), arr.Move(), alloc);
            } 
        }
    };

    template <>
    struct serde_adaptor<rapidjson_type, rttr::variant_associative_view, detail::map_t> {
        using Map = rttr::variant_associative_view;
        using E = rttr::variant;
        inline static void from(rapidjson_type& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s[key.data()];
            if (map.is_key_only_type()) {
                const rttr::type orig_key_type = map.get_key_type();
                if (detail::rttr_type_is_basic(orig_key_type)) {
                    for (size_t i=0; i<table.Size(); i++) {
                        auto& avalue = table[i];
                        E key;
                        deserialize_to(avalue, key);
                        if (key.get_type() != orig_key_type) {
                            if (!key.convert(orig_key_type)) {
                                continue;
                            }
                        }
                        map.insert(key);
                    }
                } else {
                    for (size_t i=0; i<table.Size(); i++) {
                        auto& avalue = table[i];
                        E orig_key = orig_key_type.create();
                        E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        deserialize_to(avalue, key);
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
                        for(auto& [akey, avalue] : table.GetObject()) {
                            E key;
                            deserialize_to(akey, key);

                            E value;
                            deserialize_to(avalue, value);
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
                        for(auto& [akey, avalue] : table.GetObject()) {
                            E key;
                            deserialize_to(akey, key);

                            E orig_value = orig_value_type.create();
                            rttr::type value_type1 = orig_value.get_type();
                            E value = orig_value.get_type().is_wrapper() 
                                    ? orig_value.extract_wrapped_ptr_value() : orig_value;
                            rttr::type value_type = value.get_type();
                            deserialize_to(avalue, value);
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
                        for(auto& avalue : table.GetArray()) {
                            E orig_key = orig_key_type.create(); 
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_ptr_value() : orig_key;
                                    
                            E value;
                            deserialize_to(avalue["key"], key);
                            deserialize_to(avalue["value"], value);
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
                        for(auto& avalue : table.GetArray()) {
                            E orig_key = orig_key_type.create(); 
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_ptr_value() : orig_key;
                            E orig_value = orig_value_type.create();
                            E value = orig_value.get_type().get_raw_type().is_wrapper() 
                                    ? orig_value.extract_wrapped_ptr_value() : orig_value;
                            deserialize_to(avalue["key"], key);
                            deserialize_to(avalue["value"], value);
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
        inline static void into(rapidjson_type& s, std::string_view key, const Map& data) {
            using namespace rapidjson;
            Value map;
            auto& alloc = s.GetAllocator();

            if (data.is_key_only_type()) {
                map.SetArray();
                for(auto& [it_value, _] : data) { 
                    Value v;
                    v.CopyFrom(serialize<rapidjson_type>(it_value), alloc);
                    map.PushBack(v.Move(), alloc);
                }
            } else {
                auto key_type = data.get_key_type();
                if (detail::rttr_type_is_basic(key_type.get_raw_type())) {
                    map.SetObject();
                    for(auto& [it_key, it_value] : data) {
                        bool ok = false;
                        std::string str_key = it_key.to_string(&ok);
                        if (!ok) {
                            continue;
                        }
                        Value skey(str_key.c_str(), str_key.length(), alloc);
                        Value v;
                        v.CopyFrom(serialize<rapidjson_type>(it_value), alloc);
                        map.AddMember(skey, v.Move(), alloc);
                    }
                } else {
                    map.SetArray();

                    for(auto& [it_key, it_value] : data) {
                        Value val;
                        val.SetObject();
                        {
                            Value v;
                            v.CopyFrom(serialize<rapidjson_type>(it_key), alloc);
                            val.AddMember("key", v.Move(), alloc);
                        }
                        {
                            Value v;
                            v.CopyFrom(serialize<rapidjson_type>(it_value), alloc);
                            val.AddMember("value", v.Move(), alloc);
                        }

                        map.PushBack(val.Move(), alloc);
                    }
                }
            }
            if(key.empty()) {
                s.CopyFrom(map.Move(), alloc);
            } else {
                if(!s.IsObject()) s.SetObject();
                s.AddMember(rapidjson::StringRef(key.data()), map.Move(), alloc);
            }
        }
    };

    template<>
    struct serde_adaptor<rapidjson_type, rttr::variant, detail::struct_t> {
        using Object = rttr::variant;
        static void from(rapidjson_type& s, std::string_view key, Object& orig_data) {
            rttr::type orig_type = orig_data.get_type();
            rttr::instance data(orig_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_type();

            auto& table = key.empty() ? s : s[key.data()];
            for(auto& [jkey, avalue] : table.GetObject()) { 
                auto str_key = jkey.GetString();
                rttr::property prop = data_type.get_property(str_key);
                if (prop) {
                    auto prop_name = prop.get_name();
                    rttr::variant prop_val = prop.get_value(obj);
                    deserialize_to<rttr::variant>(avalue, prop_val); 
                    prop.set_value(obj, prop_val);
                }
            }
        }
        static void into(rapidjson_type& s, std::string_view key, const Object& _data) {
            using namespace rapidjson;
            rttr::instance data(_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_derived_type();

            Value map;
            auto& alloc = s.GetAllocator();
            map.SetObject();
            for(auto& prop : data_type.get_properties()) {
                const rttr::variant prop_value = prop.get_value(obj);
                auto prop_name = prop.get_name();

                std::string str_key = prop_name.to_string();
                Value skey(str_key.c_str(), str_key.length(), alloc);
                Value v;
                v.CopyFrom(serialize<rapidjson_type>(prop_value), alloc);
                map.AddMember(skey, v.Move(), alloc);
            }
            if(key.empty()) {
                s.CopyFrom(map.Move(), alloc);
            } else {
                if(!s.IsObject()) s.SetObject();
                s.AddMember(rapidjson::StringRef(key.data()), map.Move(), alloc);
            }
        } 
    };

}

namespace serde {

    template <>
    struct serde_adaptor<rapidjson_value, rttr::variant_polymoph_view, detail::poly_t> {
        using Container = rttr::variant_polymoph_view;
        using E = rttr::variant;
        inline static void from(rapidjson_value& s, std::string_view key, Container& container) {
            auto& table = key.empty() ? s : s[key.data()];
            if (!table.IsObject()) { return; }

            auto& a_type_name = table["$typeName"];
            if (!a_type_name.IsString()) return;
            std::string type_name(a_type_name.GetString(), a_type_name.GetStringLength());
            if (type_name.size() == 0) { return; }

            auto& a_value = table["$content"];
            if (!a_value.IsObject()) { return; }
            if (!container.create(type_name)) { return; }

            rttr::variant value = container.get_value();
            deserialize_to(a_value, value);
        }
    };

    template <>
    struct serde_adaptor<rapidjson_value, rttr::variant_sequential_view, detail::seq_t> {
        using Sequent = rttr::variant_sequential_view;
        using E = rttr::variant;
        inline static void from(rapidjson_value& s, std::string_view key, Sequent& seq) {
            auto& table = key.empty() ? s : s[key.data()];
            if(!table.IsArray()) { table.SetArray(); }
            seq.set_size(table.Size());
            const rttr::type orig_value_type = seq.get_value_type();
            for (size_t i=0; i<table.Size(); i++) {
                auto& avalue = table[i];

                E orig_value = seq.get_value(i);
                rttr::type value_type1 = orig_value.get_type();
                E value = orig_value.get_type().is_wrapper() 
                        ? orig_value.extract_wrapped_ptr_value() : orig_value;
                rttr::type value_type = value.get_type();

                deserialize_to(avalue, value);

                seq.set_value(i, value);
            }
        }
    };

    template <>
    struct serde_adaptor<rapidjson_value, rttr::variant_associative_view, detail::map_t> {
        using Map = rttr::variant_associative_view;
        using E = rttr::variant;
        inline static void from(rapidjson_value& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s[key.data()];
            if (map.is_key_only_type()) {
                const rttr::type orig_key_type = map.get_key_type();
                if (detail::rttr_type_is_basic(orig_key_type)) {
                    for (size_t i=0; i<table.Size(); i++) {
                        auto& avalue = table[i];
                        E key;
                        deserialize_to(avalue, key);
                        if (key.get_type() != orig_key_type) {
                            if (!key.convert(orig_key_type)) {
                                continue;
                            }
                        }
                        map.insert(key);
                    }
                } else {
                    for (size_t i=0; i<table.Size(); i++) {
                        auto& avalue = table[i];
                        E orig_key = orig_key_type.create();
                        E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        deserialize_to(avalue, key);
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

                bool ok = false;
                if (detail::rttr_type_is_basic(orig_key_type)) {
                    if (detail::rttr_type_is_basic(orig_value_type)) {
                        for(auto& [jkey, avalue] : table.GetObject()) {

                            E key;
                            deserialize_to(jkey, key);
                            
                            E value;
                            rttr::type value_type = value.get_type();
                            deserialize_to(avalue, value);
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
                        for(auto& [jkey, avalue] : table.GetObject()) {

                            E key;
                            deserialize_to(jkey, key);
                            
                            E orig_value = orig_value_type.create();
                            rttr::type value_type1 = orig_value.get_type();
                            E value = orig_value.get_type().is_wrapper() 
                                    ? orig_value.extract_wrapped_ptr_value() : orig_value;
                            rttr::type value_type = value.get_type();
                            deserialize_to(avalue, value);
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
                        for(auto& avalue : table.GetArray()) {
                            E orig_key = orig_key_type.create(); 
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_ptr_value() : orig_key;
                            auto key_type = key.get_type();

                            E value;
                            deserialize_to(avalue["key"], key);
                            deserialize_to(avalue["value"], value);
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
                        for(auto& avalue : table.GetArray()) {
                            E orig_key = orig_key_type.create(); 
                            E key = orig_key.get_type().get_raw_type().is_wrapper() 
                                    ? orig_key.extract_wrapped_ptr_value() : orig_key;
                            auto key_type = key.get_type();
                            E orig_value = orig_value_type.create();
                            E value = orig_value.get_type().get_raw_type().is_wrapper() 
                                    ? orig_value.extract_wrapped_ptr_value() : orig_value;
                            deserialize_to(avalue["key"], key);
                            deserialize_to(avalue["value"], value);
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
    };

    template<>
    struct serde_adaptor<rapidjson_value, rttr::variant, detail::struct_t> {
        using Object = rttr::variant;
        static void from(rapidjson_value& s, std::string_view key, Object& orig_data) {
            rttr::type orig_type = orig_data.get_type();
            rttr::instance data(orig_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() 
                    ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_type();

            auto& table = key.empty() ? s : s[key.data()];
            auto tabType = table.GetType();
            for(auto& [jkey, avalue] : table.GetObject()) { 
                rttr::property prop = data_type.get_property(jkey.GetString());
                if (prop) {
                    rttr::variant prop_val = prop.get_value(obj);
                    deserialize_to<rttr::variant>(avalue, prop_val); 
                    prop.set_value(obj, prop_val);
                }
            }
        }
    };

}