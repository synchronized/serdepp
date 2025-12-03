#pragma once

#ifndef __SERDEPP_DETAIL_EXTEND_RTTR_FMT_IMPL_HPP__
#define __SERDEPP_DETAIL_EXTEND_RTTR_FMT_IMPL_HPP__

#include "serdepp/detail/extend/rttr/sstream_impl.hpp"

template <typename FormatCtx>
auto fmt::formatter<rttr::variant>::format(const rttr::variant &serde_type, FormatCtx &ctx) const -> decltype(ctx.out()) {
  return fmt::formatter<std::string>::format(serde::to_string(serde_type), ctx);
}

#endif