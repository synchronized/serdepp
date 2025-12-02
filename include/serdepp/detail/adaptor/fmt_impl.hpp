#pragma once

#ifndef __SERDEPP_ADAPTOR_DETAIL_FMT_IMPL_HPP__
#define __SERDEPP_ADAPTOR_DETAIL_FMT_IMPL_HPP__

#include "serdepp/detail/adaptor/sstream_impl.hpp"

template <typename T>
template <typename FormatCtx>
auto fmt::formatter<T, std::enable_if_t<serde::detail::is_struct_v<T>, char>>::format(
    const T &serde_type, FormatCtx &ctx) const -> decltype(ctx.out())
{
    return fmt::formatter<std::string>::format(serde::to_string(serde_type), ctx);
}

#endif
