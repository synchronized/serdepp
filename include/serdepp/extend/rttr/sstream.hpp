#pragma once

#include <serdepp/serde.hpp>
#include <serdepp/adaptor/sstream.hpp>

#include <serdepp/extend/rttr.hpp>

#include <rttr/type>

namespace serde {
    template <>
    struct serde_adaptor<serde_sstream, rttr::variant, type::basic_t> {
        using ValueType = rttr::variant;
        inline static void from(serde_sstream& s, std::string_view key, ValueType& data) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        inline static void into(serde_sstream& s, std::string_view key, const ValueType& data) {
            const rttr::type data_type = data.get_type();
            const rttr::type wrapped_type = data_type.is_wrapper() ? data_type.get_wrapped_type() : data_type;

            if (wrapped_type.is_arithmetic()) {
                s.add(data.to_string(), key); 
                return;
            } 

            else if (wrapped_type.is_enumeration()) {
                bool ok;
                std::string str_value = data.to_string(&ok);
                if (ok) {
                    s.add(str_value, key); 
                    return;
                }
            } 

            else if (data_type == rttr::type::get<std::string>()) {
                s.add(data.to_string(), key); 
                return;
            }

            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                s.add(str_value, key); 
                return;
            }

        }
    };

    template <>
    struct serde_adaptor<serde_sstream, rttr::variant_polymoph_view, type::poly_t> {
        using Container = rttr::variant_polymoph_view;
        using E = rttr::variant;
        inline static void from(serde_sstream& s, std::string_view key, Container& container) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        inline static void into(serde_sstream& s, std::string_view key, const Container& data) {
            if (!data.is_valid()) { return; }

            if(key.empty()) {
                s.set_wrapper('[', ']');
                s.add(data.get_type_name(), "$typeName");
                rttr::variant value = data.get_value();
                s.add(serialize<serde_sstream>(value).str(), "$content");
            } else {
                serde_sstream ss('[',']');
                ss.add(data.get_type_name(), "$typeName");
                rttr::variant value = data.get_value();
                ss.add(serialize<serde_sstream>(value).str(), "$content");
                s.add(ss.str(), key);
            }
        }
    };

    template <>
    struct serde_adaptor<serde_sstream, rttr::variant_sequential_view, type::seq_t> {
        using Sequent = rttr::variant_sequential_view;
        using E = rttr::variant;
        inline static void from(serde_sstream& s, std::string_view key, Sequent& seq) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        inline static void into(serde_sstream& s, std::string_view key, const Sequent& data) {
            if(key.empty()) {
                s.set_wrapper('[', ']');
                for(auto& it : data) { s.add(serialize<serde_sstream>(it).str()); }
            } else {
                serde_sstream ss('[',']');
                for(auto& it : data) { ss.add(serialize<serde_sstream>(it).str()); }
                s.add(ss.str(), key);
            }
        }
    };

    template <>
    struct serde_adaptor<serde_sstream, rttr::variant_associative_view, type::map_t> {
        using Map = rttr::variant_associative_view;
        using E = rttr::variant;
        inline static void from(serde_sstream& s, std::string_view key, Map& map) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        inline static void into(serde_sstream& s, std::string_view key, const Map& data) {
            if (data.is_key_only_type()) {
                if(key.empty()) {
                    s.set_wrapper('{', '}');
                    for(auto& [it_key, _] : data) { s.add(serialize<serde_sstream>(it_key).str()); }
                } else {
                    serde_sstream ss('{','}');
                    for(auto& [it_key, _] : data) { ss.add(serialize<serde_sstream>(it_key).str()); }
                    s.add(ss.str(), key);
                }
            } else {
                if(key.empty()) {
                    s.set_wrapper('{', '}');
                    auto orig_key_type = data.get_key_type();
                    auto orig_value_type = data.get_value_type();
                    for(auto& [it_key, it_value] : data) { 
                        auto key_type = it_key.get_type();
                        auto value_type = it_value.get_type();
                        s.add(serialize<serde_sstream>(it_value).str(), serialize<serde_sstream>(it_key).str()); 
                    }
                } else {
                    serde_sstream ss('{','}');
                    for(auto& [it_key, it_value] : data) { 
                        ss.add(serialize<serde_sstream>(it_value).str(), serialize<serde_sstream>(it_key).str()); 
                    }
                    s.add(ss.str(), key);
                }
            }
        }
    };

    template<>
    struct serde_adaptor<serde_sstream, rttr::variant, type::struct_t> {
        using Object = rttr::variant;
        static void from(serde_sstream& s, std::string_view key, Object& orig_data) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        static void into(serde_sstream& s, std::string_view key, const Object& _data) {
            rttr::instance data(_data);
            rttr::instance obj = data.get_type().get_raw_type().is_wrapper() ? data.get_wrapped_instance() : data;
            rttr::type data_type = obj.get_derived_type();

            if(key.empty()) {
                for(auto& prop : data_type.get_properties()) {
                    const rttr::variant prop_value = prop.get_value(obj);
                    std::string prop_name = prop.get_name().to_string();
                    s.add(serialize<serde_sstream>(prop_value).str(), prop_name); 
                }
            } else {
                serde_sstream ss;
                for(auto& prop : data_type.get_properties()) {
                    const rttr::variant prop_value = prop.get_value(obj);
                    auto prop_name = prop.get_name().to_string();
                    ss.add(serialize<serde_sstream>(prop_value).str(), prop_name); 
                }
                s.add(ss.str(), key);
            }
        } 
    };
}
