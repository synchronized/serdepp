#pragma once

#ifndef __SERDEPP_DETAIL_EXTEND_RTTR_FMT_DEFINE_HPP__
#define __SERDEPP_DETAIL_EXTEND_RTTR_FMT_DEFINE_HPP__

#include <rttr/type>

#include "serdepp/detail/extend/rttr/sstream_define.hpp"

template <>
struct fmt::formatter<rttr::variant> : fmt::formatter<std::string> {
    template <typename FormatCtx>
    auto format(const rttr::variant &serde_type, FormatCtx &ctx) const -> decltype(ctx.out());
};

#endif