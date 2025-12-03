#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_SSTREAM_IMPL_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_SSTREAM_IMPL_HPP__

namespace serde {

// --------------- serde_sstream ---------------
serde_sstream::serde_sstream(char begin, char end) : members(), begin_(begin), end_(end) {}
void serde_sstream::set_wrapper(char begin, char end) { begin_ = begin; end_ = end; }

serde_sstream & serde_sstream::add(const std::string &data, std::string_view key) {
    members.push_back(key.empty() ? data : "\"" + std::string{key} + "\": " + data);
    return *this;
}

inline std::string serde_sstream::str() const {
    if (members.size() == 0) {
        return "";
    }
    if (members.size() == 1) {
        return members.front();
    } 
    std::stringstream s;
    s << begin_;
    s << members[0];
    for (size_t i=1; i<members.size(); i++) { 
        s << ", " << members[i]; 
    }
    s << end_;
    return s.str();
}

// --------------- serde_adaptor_helper ---------------
inline constexpr bool serde_adaptor_helper<serde_sstream>::is_null(serde_sstream &adaptor, std::string_view key) { return false; }

inline constexpr size_t serde_adaptor_helper<serde_sstream>::size(serde_sstream &adaptor) { return 1; }

inline constexpr bool serde_adaptor_helper<serde_sstream>::is_struct(serde_sstream &adaptor) { return true; }


// --------------- serde_adaptor<serde_sstream, T> ---------------
template<typename T> 
void serde_adaptor<serde_sstream, T>::from(serde_sstream& s, std::string_view key, T& data){
    throw serde::unimplemented_error("serde_adaptor<{}>::from(serde_sstream, key data)"
                                        + std::string(nameof::nameof_short_type<serde_sstream>()));
}

template<typename T> 
void serde_adaptor<serde_sstream, T>::into(serde_sstream& s, std::string_view key, const T& data) {
    std::stringstream ss; ss << data;
    s.add(ss.str(), key);
}


// --------------- serde_adaptor<serde_sstream, T, detail::struct_t> ---------------
template<typename T> 
void serde_adaptor<serde_sstream, T, detail::struct_t>::from(serde_sstream& s, std::string_view key, T& data) {
    throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
}

template<typename T> 
void serde_adaptor<serde_sstream, T, detail::struct_t>::into(serde_sstream &s, std::string_view key, const T& data) {
    s.add(serialize<serde_sstream>(data).str(), key);
} 

// --------------- struct serde_adaptor<serde_sstream, T, detail::seq_t> ---------------
template<typename T> 
void serde_adaptor<serde_sstream, T, detail::seq_t>::from(serde_sstream& s, std::string_view key, T& arr) {
    throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
}

template<typename T> 
void serde_adaptor<serde_sstream, T, detail::seq_t>::into(serde_sstream& s, std::string_view key, const T& data) {
    if(key.empty()) {
        s.set_wrapper('[', ']');
        for(auto& it : data) { s.add(serialize<serde_sstream>(it).str()); }
    } else {
        serde_sstream ss('[',']');
        for(auto& it : data) { ss.add(serialize<serde_sstream>(it).str()); }
        s.add(ss.str(), key);
    }
}

// --------------- struct serde_adaptor<serde_sstream, T, detail::seq_t> ---------------
template <typename Map> 
void serde_adaptor<serde_sstream, Map, detail::map_t>::from(serde_sstream& s, std::string_view key, Map& map) {
    throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
}
template <typename Map> 
void serde_adaptor<serde_sstream, Map, detail::map_t>::into(serde_sstream& s, std::string_view key, const Map& data) {
    if(key.empty()) {
        for(auto& [key_, it] : data) { s.add(serialize<serde_sstream>(it).str(), key_); }
    } else {
        serde_sstream ss;
        for(auto& [key_, it] : data) { ss.add(serialize<serde_sstream>(it).str(), key_); }
        s.add(ss.str(), key);
    }
}

template<typename T>
std::string to_string(const T& type) { return serialize<serde_sstream>(type).str(); }

}

#endif
