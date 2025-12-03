#pragma once


namespace serde {

// --------------- serde_adaptor<YAML::Node, rttr::variant, detail::basic_t> ---------------
void serde_adaptor<YAML::Node, rttr::variant, detail::basic_t>::from(yaml& s, std::string_view key, ValueType& data) {
    ValueType val;
    if (key.empty()) {
        auto& avalue = s;

        if (avalue.IsNull()) {
            return;
        }
        else if (avalue.IsScalar()) {
            std::string str_val = avalue.Scalar();
            val = str_val;
        }
    } else {
        auto avalue = s[std::string{key}];

        if (avalue.IsNull()) {
            return;
        }
        else if (avalue.IsScalar()) {
            std::string str_val = avalue.Scalar();
            val = str_val;
        }
    }
    if (val) {
        const rttr::type val_type = val.get_type();
        const rttr::type data_type = data.get_type();
        if (!data || val.convert(data_type.get_raw_type())) {
            data = val;
        }
    }

}
void serde_adaptor<YAML::Node, rttr::variant, detail::basic_t>::into(yaml& s, std::string_view key, const ValueType& orig_data) {
    const rttr::type orig_data_type = orig_data.get_type();
    rttr::variant wrapped_val = orig_data_type.is_wrapper() 
            ? orig_data.extract_wrapped_ref_value() : orig_data;
    const rttr::type wrapped_type = wrapped_val.get_type();
    rttr::variant data = wrapped_type.is_pointer() 
            ? wrapped_val.extract_pointer_value() : wrapped_val;
    const rttr::type data_type = data.get_type();

    if (key.empty()) {
        auto& avalue = s;

        if (data_type.is_arithmetic()) {
            avalue = data.to_string();
        } 

        else if (data_type.is_enumeration()) {
            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                avalue = str_value;
                return;
            }
        } 

        else if (data_type == rttr::type::get<std::string>()) {
            avalue = data.to_string();
            return;
        } 

        bool ok;
        std::string str_value = data.to_string(&ok);
        if (ok) {
            avalue = str_value; 
            return;
        }
    } else {
        auto avalue = s[std::string{key}];

        if (data_type.is_arithmetic()) {
            avalue = data.to_string();
        } 

        else if (data_type.is_enumeration()) {
            bool ok;
            std::string str_value = data.to_string(&ok);
            if (ok) {
                avalue = str_value;
                return;
            }
        } 

        else if (data_type == rttr::type::get<std::string>()) {
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
}

// --------------- serde_adaptor<YAML::Node, rttr::variant_polymoph_view, detail::poly_t> ---------------
void serde_adaptor<YAML::Node, rttr::variant_polymoph_view, detail::poly_t>::from(yaml& s, std::string_view key, Container& container) {

    if(key.empty()) {
        auto& table = s;
        std::string type_name = table["$typeName"].Scalar();
        if (!container.create(type_name)) return;

        rttr::variant value = container.get_value();
        deserialize_to(table["$content"], value);
    } else {
        auto table = s[std::string{key}];

        std::string type_name = table["$typeName"].Scalar();
        if (!container.create(type_name)) return;

        rttr::variant value = container.get_value();
        deserialize_to(table["$content"], value);
    }
}
void serde_adaptor<YAML::Node, rttr::variant_polymoph_view, detail::poly_t>::into(yaml& s, std::string_view key, const Container& data) {
    if (!data.is_valid()) return;
    if(key.empty()) {
        auto& obj = s;
        std::string type_name = data.get_real_type().get_name().to_string();
        obj["$typeName"] = serialize<yaml>(type_name);
        rttr::variant value = data.get_value();
        obj["$content"] = serialize<yaml>(value);
    } else {
        auto obj = s[std::string{key}];
        std::string type_name = data.get_real_type().get_name().to_string();
        obj["$typeName"] = serialize<yaml>(type_name);
        rttr::variant value = data.get_value();
        obj["$content"] = serialize<yaml>(value);
    }
}

// --------------- serde_adaptor<YAML::Node, rttr::variant_sequential_view, detail::seq_t> ---------------
void serde_adaptor<YAML::Node, rttr::variant_sequential_view, detail::seq_t>::from(yaml& s, std::string_view key, Sequent& seq) {
    if(key.empty()) {
        auto& table = s;
        seq.set_size(table.size());
        for(std::size_t i = 0 ; i < table.size(); ++i) { 
            auto jvalue = table[i];

            E orig_value = seq.get_value(i);
            rttr::type value_type1 = orig_value.get_type();
            E value = orig_value.get_type().is_wrapper() ? orig_value.extract_wrapped_ptr_value() : orig_value;
            rttr::type value_type = value.get_type();

            deserialize_to(jvalue, value);

            seq.set_value(i, value);
        }
    } else {
        auto table = s[std::string{key}];
        seq.set_size(s.size());
        for(std::size_t i = 0 ; i < table.size(); ++i) { 
            auto jvalue = table[i];

            E orig_value = seq.get_value(i);
            rttr::type value_type1 = orig_value.get_type();
            E value = orig_value.get_type().is_wrapper() ? orig_value.extract_wrapped_ptr_value() : orig_value;
            rttr::type value_type = value.get_type();

            deserialize_to(jvalue, value);

            seq.set_value(i, value);
        }
    }
}
void serde_adaptor<YAML::Node, rttr::variant_sequential_view, detail::seq_t>::into(yaml& s, std::string_view key, const Sequent& data) {
    if(key.empty()) {
        auto& arr = s;
        for(auto& value: data) { arr.push_back(serialize<yaml>(value)); }
    } else {
        auto arr = s[std::string{key}];
        for(auto& value: data) { arr.push_back(serialize<yaml>(value)); }
    }
}

// --------------- serde_adaptor<YAML::Node, rttr::variant_associative_view, detail::map_t> ---------------
void serde_adaptor<YAML::Node, rttr::variant_associative_view, detail::map_t>::from(yaml& s, std::string_view key, Map& map) {
    if (key.empty()) {
        auto& arr = s;

        if (map.is_key_only_type()) {
            const rttr::type orig_key_type = map.get_key_type();
            if (detail::rttr_type_is_basic(orig_key_type)) {
                for(std::size_t i=0; i<arr.size(); ++i) {
                    auto jvalue = arr[i];
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
                for(std::size_t i=0; i<arr.size(); ++i) {
                    auto jvalue = arr[i];
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
            bool ok = false;
            if (detail::rttr_type_is_basic(orig_key_type)) {
                if (detail::rttr_type_is_basic(orig_value_type)) {
                    for(auto it = arr.begin(); it!=arr.end(); ++it) {
                        auto jkey = it->first, jvalue = it->second;

                        E key;
                        deserialize_to(jkey, key);
                        rttr::type key_type = key.get_type();

                        E value;
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
                } else {
                    for(auto it = arr.begin(); it!=arr.end(); ++it) {
                        auto jkey = it->first, jvalue = it->second;

                        E key;
                        deserialize_to(jkey, key);

                        E orig_value = orig_value_type.create();
                        rttr::type value_type1 = orig_value.get_type();
                        E value = orig_value.get_type().is_wrapper() ? orig_value.extract_wrapped_ptr_value() : orig_value;
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
            else {
                if (detail::rttr_type_is_basic(orig_value_type)) {
                    for(std::size_t i=0; i<arr.size(); ++i) {
                        auto jvalue = arr[i];
                        E orig_key = orig_key_type.create(); 
                        rttr::type key_type1 = orig_key.get_type();
                        E key = key_type1.get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        rttr::type key_type = key.get_type();

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
                    for(std::size_t i=0; i<arr.size(); ++i) {
                        auto jvalue = arr[i];
                        E orig_key = orig_key_type.create(); 
                        rttr::type key_type1 = orig_key.get_type();
                        E key = key_type1.get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        rttr::type key_type = key.get_type();
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

    } else {
        auto arr = s[std::string{key}];

        if (map.is_key_only_type()) {
            const rttr::type orig_key_type = map.get_key_type();
            if (detail::rttr_type_is_basic(orig_key_type)) {
                for(std::size_t i=0; i<arr.size(); ++i) {
                    auto jvalue = arr[i];
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
                for(std::size_t i=0; i<arr.size(); ++i) {
                    auto jvalue = arr[i];
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
            bool ok = false;
            if (detail::rttr_type_is_basic(orig_key_type)) {
                if (detail::rttr_type_is_basic(orig_value_type)) {
                    for(auto it = arr.begin(); it!=arr.end(); ++it) {
                        auto jkey = it->first, jvalue = it->second;

                        E key;
                        deserialize_to(jkey, key);

                        E value;
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
                } else {
                    for(auto it = arr.begin(); it!=arr.end(); ++it) {
                        auto jkey = it->first, jvalue = it->second;

                        E key;
                        deserialize_to(jkey, key);

                        E orig_value = orig_value_type.create();
                        rttr::type value_type1 = orig_value.get_type();
                        E value = orig_value.get_type().is_wrapper() ? orig_value.extract_wrapped_ptr_value() : orig_value;
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
            else {
                if (detail::rttr_type_is_basic(orig_value_type)) {
                    for(std::size_t i=0; i<arr.size(); ++i) {
                        auto jvalue = arr[i];
                        E orig_key = orig_key_type.create(); 
                        rttr::type key_type1 = orig_key.get_type();
                        E key = key_type1.get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        rttr::type key_type = key.get_type();

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
                    for(std::size_t i=0; i<arr.size(); ++i) {
                        auto jvalue = arr[i];
                        E orig_key = orig_key_type.create(); 
                        rttr::type key_type1 = orig_key.get_type();
                        E key = key_type1.get_raw_type().is_wrapper() 
                                ? orig_key.extract_wrapped_ptr_value() : orig_key;
                        rttr::type key_type = key.get_type();
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
}
void serde_adaptor<YAML::Node, rttr::variant_associative_view, detail::map_t>::into(yaml& s, std::string_view key, const Map& data) {
    if (key.empty()) {
        auto& map = s;

        if (data.is_key_only_type()) {
            for(auto& [key_, _] : data) { 
                map.push_back(std::move(serialize<yaml>(key_))); 
            }
        } else {
            auto key_type = data.get_key_type();
            if (detail::rttr_type_is_basic(key_type.get_raw_type())) {
                for(auto& [key_, value_] : data) { 
                    bool ok = false;
                    auto str_key = key_.to_string(&ok);
                    if (!ok) {
                        continue;
                    }
                    map[str_key] = serialize<yaml>(value_); 
                }
            } else {
                for(auto& [key_, value_] : data) { 
                    yaml obj_value; 
                    obj_value["key"] = serialize<yaml>(key_);
                    obj_value["value"] = serialize<yaml>(value_);
                    map.push_back(obj_value); 
                }
            }
        }
    } else {
        auto map = s[std::string{key}];

        if (data.is_key_only_type()) {
            for(auto& [key_, _] : data) { 
                map.push_back(std::move(serialize<yaml>(key_))); 
            }
        } else {
            auto key_type = data.get_key_type();
            if (detail::rttr_type_is_basic(key_type.get_raw_type())) {
                for(auto& [key_, value_] : data) { 
                    bool ok = false;
                    auto str_key = key_.to_string(&ok);
                    if (!ok) {
                        continue;
                    }
                    map[str_key] = serialize<yaml>(value_); 
                }
            } else {
                for(auto& [key_, value_] : data) { 
                    yaml obj_value; 
                    obj_value["key"] = serialize<yaml>(key_);
                    obj_value["value"] = serialize<yaml>(value_);
                    map.push_back(obj_value); 
                }
            }
        }
    }
}

// --------------- serde_adaptor<YAML::Node, rttr::variant, detail::basic_t> ---------------
void serde_adaptor<YAML::Node, rttr::variant, detail::struct_t>::from(yaml& s, std::string_view key, Object& orig_data) {
    rttr::type orig_type = orig_data.get_type();
    rttr::instance data(orig_data);
    rttr::instance obj = data.get_type().get_raw_type().is_wrapper() ? data.get_wrapped_instance() : data;
    rttr::type data_type = obj.get_type();

    if (key.empty()) {
        auto& table = s;
        for(auto it = table.begin(); it!=table.end(); ++it) {
            auto jkey = it->first, jvalue = it->second;
            std::string prop_name = jkey.Scalar();
            rttr::property prop = data_type.get_property(prop_name);
            if (prop) {
                rttr::variant prop_val = prop.get_value(obj);
                deserialize_to<rttr::variant>(jvalue, prop_val); 
                prop.set_value(obj, prop_val);
            }
        }
    } else {
        auto table = s[std::string{key}];
        for(auto it = table.begin(); it!=table.end(); ++it) {
            auto jkey = it->first, jvalue = it->second;
            std::string prop_name = jkey.Scalar();
            rttr::property prop = data_type.get_property(prop_name);
            if (prop) {
                rttr::variant prop_val = prop.get_value(obj);
                deserialize_to<rttr::variant>(jvalue, prop_val); 
                prop.set_value(obj, prop_val);
            }
        }

    }
}
void serde_adaptor<YAML::Node, rttr::variant, detail::struct_t>::into(yaml& s, std::string_view key, const Object& _data) {
    rttr::instance data(_data);
    rttr::instance obj = data.get_type().get_raw_type().is_wrapper() ? data.get_wrapped_instance() : data;
    rttr::type data_type = obj.get_derived_type();

    if (key.empty()) {
        auto& table = s;
        for(auto& prop : data_type.get_properties()) {
            const rttr::variant prop_value = prop.get_value(obj);
            auto prop_name = prop.get_name();
            table[std::string{prop_name}] = serialize<yaml>(prop_value);
        }
    } else {
        auto table = s[std::string{key}];
        for(auto& prop : data_type.get_properties()) {
            const rttr::variant prop_value = prop.get_value(obj);
            auto prop_name = prop.get_name();
            table[std::string{prop_name}] = serialize<yaml>(prop_value);
        }
    }
} 

}