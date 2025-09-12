#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <serdepp/adaptor/sstream.hpp>
#include <serdepp/extend/rttr/sstream.hpp>

#include <rttr/type>

template <>
struct fmt::formatter<rttr::variant, char> : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const rttr::variant &serde_type, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(serde::to_string(serde_type),
                                               ctx);
  }
};
//
// template<>
// struct fmt::formatter<serde::serde_sstream> : fmt::formatter<std::string> {
//    template <typename FormatCtx>
//    auto format(const serde::serde_sstream& serde_type, FormatCtx& ctx) {
//        return fmt::formatter<std::string>::format(serde_type.str(), ctx);
//    }
//};

