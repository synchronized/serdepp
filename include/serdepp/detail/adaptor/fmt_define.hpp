#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_FMT_DEFINE_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_FMT_DEFINE_HPP__

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "serdepp/detail/adaptor/sstream_define.hpp"

template <typename T>
struct fmt::formatter<T, std::enable_if_t<serde::detail::is_struct_v<T>, char>>
    : fmt::formatter<std::string> 
{
  template <typename FormatCtx>
  auto format(const T &serde_type, FormatCtx &ctx) const -> decltype(ctx.out());
};

#endif
