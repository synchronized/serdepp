#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_RAPID_JSON_IMPL_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_RAPID_JSON_IMPL_HPP__

namespace serde {

std::string to_string(rapidjson::Document &doc) {
    using namespace rapidjson;
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

}

namespace serde {

// --------------- serde_type_checker<rapidjson::Document> ---------------
bool serde_type_checker<rapidjson::Document>::is_integer(Format& format) { return format.IsInt(); }
bool serde_type_checker<rapidjson::Document>::is_sequence(Format& format) { return format.IsArray(); }
bool serde_type_checker<rapidjson::Document>::is_map(Format& format) { return format.IsObject(); }
bool serde_type_checker<rapidjson::Document>::is_float(Format& format) { return format.IsFloat(); }
bool serde_type_checker<rapidjson::Document>::is_string(Format& format) { return format.IsString(); }
bool serde_type_checker<rapidjson::Document>::is_bool(Format& format) { return format.IsBool(); }
bool serde_type_checker<rapidjson::Document>::is_null(Format& format) { return format.IsNull(); }
bool serde_type_checker<rapidjson::Document>::is_struct(Format& format) { return format.IsObject(); }

// --------------- serde_type_checker<rapidjson::Value> ---------------
bool serde_type_checker<rapidjson::Value>::is_integer(Format& format) { return format.IsInt(); }
bool serde_type_checker<rapidjson::Value>::is_sequence(Format& format) { return format.IsArray(); }
bool serde_type_checker<rapidjson::Value>::is_map(Format& format) { return format.IsObject(); }
bool serde_type_checker<rapidjson::Value>::is_float(Format& format) { return format.IsFloat(); }
bool serde_type_checker<rapidjson::Value>::is_string(Format& format) { return format.IsString(); }
bool serde_type_checker<rapidjson::Value>::is_bool(Format& format) { return format.IsBool(); }
bool serde_type_checker<rapidjson::Value>::is_null(Format& format) { return format.IsNull(); }
bool serde_type_checker<rapidjson::Value>::is_struct(Format& format) { return format.IsObject(); }

// --------------- serde_adaptor_helper<rapidjson::Document> ---------------
rapidjson::Document serde_adaptor_helper<rapidjson::Document>::parse_file(const std::string& path) {
    using namespace rapidjson;
    Document doc;
    std::ifstream ifs{path};
    IStreamWrapper isw{ifs};
    doc.ParseStream(isw);
    return doc;
}
bool serde_adaptor_helper<rapidjson::Document>::is_null(rapidjson_type& adaptor, std::string_view key) {
    return !adaptor.HasMember(key.data());
}
size_t serde_adaptor_helper<rapidjson::Document>::size(rapidjson_type& adaptor)    {
    return std::distance(adaptor.GetObject().begin(), adaptor.GetObject().end());
}
bool serde_adaptor_helper<rapidjson::Document>::is_struct(rapidjson_type& adaptor) { 
    return adaptor.IsObject(); 
}

// --------------- serde_adaptor_helper<rapidjson::Value> ---------------
bool serde_adaptor_helper<rapidjson::Value>::is_null(rapidjson::Value& adaptor, std::string_view key) {
    return !adaptor.HasMember(key.data());
}
size_t serde_adaptor_helper<rapidjson::Value>::size(rapidjson::Value& adaptor) {
    return std::distance(adaptor.GetObject().begin(), adaptor.GetObject().end());
}
bool serde_adaptor_helper<rapidjson::Value>::is_struct(rapidjson::Value& adaptor) { 
    return adaptor.IsObject(); 
}

// --------------- serde_adaptor<rapidjson::Document, T> ---------------
template<typename T>
constexpr void serde_adaptor<rapidjson::Document, T>::from(rapidjson_type& s, std::string_view key, T& data) {
    data = key.empty() ? s.Get<T>() : s[key.data()].Get<T>(); 
}

template<typename T>
void serde_adaptor<rapidjson::Document, T>::into(rapidjson_type& s, std::string_view key, const T& data) {
    if(key.empty()) {
        s.Set(data);
    } else {
        if(!s.IsObject()) s.SetObject();
        rapidjson::Value v;
        v.Set(data);
        s.AddMember(rapidjson::StringRef(key.data()), v.Move(), s.GetAllocator());
    }
}

// --------------- serde_adaptor<rapidjson::Value, T> ---------------
template<typename T>
void serde_adaptor<rapidjson::Value, T>::from(rapidjson::Value& s, std::string_view key, T& data) {
    data = key.empty() ? s.Get<T>() : s[key.data()].Get<T>(); 
}

// --------------- serde_adaptor<rapidjson::Value, std::string> ---------------
void serde_adaptor<rapidjson::Value, std::string>::from(rapidjson::Value& s, std::string_view key, std::string& data) {
    data = key.empty() ? s.GetString() : s[key.data()].GetString(); 
}

// --------------- serde_adaptor<rapidjson::Value, std::string> ---------------
void serde_adaptor<rapidjson::Document, std::string>::from(rapidjson_type& s, std::string_view key, std::string& data) {
    data = key.empty() ? s.GetString() : s[key.data()].GetString(); 
}

void serde_adaptor<rapidjson::Document, std::string>::into(rapidjson_type& s, std::string_view key, const std::string& data) {
    if(key.empty()) {
        s.SetString(data.c_str(), data.length(), s.GetAllocator());
    } else {
        if(!s.IsObject()) s.SetObject();
        rapidjson::Value buf;
        buf.SetString(data.c_str(), data.length(), s.GetAllocator());
        s.AddMember(rapidjson::StringRef(key.data()), buf.Move(), s.GetAllocator());
    }
}

// --------------- serde_adaptor<rapidjson::Document, T, detail::struct_t> ---------------
template<typename T>
void serde_adaptor<rapidjson::Document, T, detail::struct_t>::from(rapidjson_type& s, std::string_view key, T& data) {
    using namespace rapidjson;
    auto& obj = s[key.data()];
    if(!obj.IsObject()) return;
    deserialize_to(obj, data);
}
template<typename T>
void serde_adaptor<rapidjson::Document, T, detail::struct_t>::into(rapidjson_type& s, std::string_view key, const T& data) {
    using namespace rapidjson;
    Value buf;
    buf.CopyFrom(serialize<rapidjson_type>(data).Move(), s.GetAllocator());
    s.AddMember(rapidjson::StringRef(key.data()), buf.Move(), s.GetAllocator());
} 

// --------------- serde_adaptor<rapidjson::Value, T, detail::struct_t> ---------------
template<typename T>
void serde_adaptor<rapidjson::Value, T, detail::struct_t>::from(rapidjson::Value& s, std::string_view key, T& data) {
    using namespace rapidjson;
    auto& obj = s[key.data()];
    if(!obj.IsObject()) return;
    deserialize_to(obj, data);
}

// --------------- serde_adaptor<rapidjson::Document, T, detail::seq_t> ---------------
template<typename T>
void serde_adaptor<rapidjson::Document, T, detail::seq_t>::from(rapidjson_type& s, std::string_view key, T& arr) {
    using namespace rapidjson;
    auto& table = key.empty() ? s : s[key.data()];
    if(!table.IsArray()) { return; }
    if constexpr(detail::is_reserveable_v<T>) arr.reserve(table.Size());
    for(unsigned int i = 0; i < table.Size(); ++i) {
        arr.push_back(std::move(deserialize<E>(table[i])));
    }
}

template<typename T>
void serde_adaptor<rapidjson::Document, T, detail::seq_t>::into(rapidjson_type& s, std::string_view key, const T& data) {
    using namespace rapidjson;
    auto& alloc = s.GetAllocator();
    Value arr;
    arr.SetArray();
    arr.Reserve(data.size(), alloc);
    for(auto& value: data) {
        Value v;
        v.CopyFrom(serialize<rapidjson_type>(value), alloc);
        arr.PushBack(v.Move(), alloc);
    }
    if(key.empty()) {
        s.CopyFrom(arr.Move(), alloc);
    }else {
        if(!s.IsObject()) s.SetObject();
        s.AddMember(StringRef(key.data()), arr.Move(), alloc);
    } 
}

// --------------- serde_adaptor<rapidjson::Value, T, detail::seq_t> ---------------
template<typename T>
void serde_adaptor<rapidjson::Value, T, detail::seq_t>::from(rapidjson::Value& s, std::string_view key, T& arr) {
    using namespace rapidjson;
    auto& table = key.empty() ? s : s[key.data()];
    if(!table.IsArray()) { return; }
    if constexpr(detail::is_reserveable_v<T>) arr.reserve(table.Size());
    for(unsigned int i = 0; i < table.Size(); ++i) {
        arr.push_back(std::move(deserialize<E>(table[i])));
    }
}

// --------------- serde_adaptor<rapidjson::Document, Map, detail::map_t> ---------------
template <typename Map>
void serde_adaptor<rapidjson::Document, Map, detail::map_t>::from(rapidjson_type& s, std::string_view key, Map& map) {
    using namespace rapidjson;
    auto& table = key.empty() ? s : s[key.data()];
    for(auto& [key_, value_] : table.GetObject()) {
        std::string k = key_.GetString();
        deserialize_to<E>(value_, map[k]);
    }
}
template <typename Map>
void serde_adaptor<rapidjson::Document, Map, detail::map_t>::into(rapidjson_type& s, std::string_view key, const Map& data) {
    using namespace rapidjson;
    Value map;
    map.SetObject();
    for(auto& [key_, value_] : data) {
        Value val;
        Value skey(key_.c_str(), key_.length(), s.GetAllocator());
        val.CopyFrom(serialize<rapidjson_type>(value_), s.GetAllocator());
        map.AddMember(skey, val.Move(), s.GetAllocator());
    }
    if(key.empty()) {
        s.CopyFrom(map.Move(), s.GetAllocator());
    } else {
        if(!s.IsObject()) s.SetObject();
        s.AddMember(StringRef(key.data()), map.Move(), s.GetAllocator());
    }
}

// --------------- serde_adaptor<rapidjson::Value, Map, detail::map_t> ---------------
template <typename Map>
void serde_adaptor<rapidjson::Value, Map, detail::map_t>::from(rapidjson::Value& s, std::string_view key, Map& map) {
    using namespace rapidjson;
    auto& table = key.empty() ? s : s[key.data()];
    for(auto& [key_, value_] : table.GetObject()) {
        std::string k = key_.GetString();
        deserialize_to<E>(value_, map[k]);
    }
}

}

#endif
