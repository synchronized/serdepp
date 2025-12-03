#pragma once

#ifndef __SERDEPP_DETAIL_IMPL_SERIALIZER_IMPL_HPP__
#define __SERDEPP_DETAIL_IMPL_SERIALIZER_IMPL_HPP__

#include <string_view>

namespace serde
{

using namespace std::string_view_literals;

template<class Adaptor>
constexpr Adaptor parse_file(const std::string& path) {
    return serde_adaptor_helper<Adaptor>::parse_file(path);
}

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    Adaptor(key) -> T or Adaptor -> T
    T t = deserialize<T>(Adaptor, key)
    */
template <typename T, class Adaptor>
T deserialize(Adaptor&& adaptor, std::string_view key) {
    using origin = detail::remove_cvref_t<Adaptor>;
    T target;
    if constexpr(std::is_pointer_v<T>) { target = nullptr; }
    serde_context<origin> ctx(adaptor);
    serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
    return target;
}

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    Adaptor[key] -> T or Adaptor -> T
    deserialize_to(Adaptor, T, key)
    */
template<typename T, class Adaptor>
void deserialize_to(Adaptor&& adaptor, T& target, std::string_view key) {
    using origin = detail::remove_cvref_t<Adaptor>;
    serde_context<origin> ctx(adaptor);
    serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
}

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    T -> Adaptor[key] or T -> Adaptor
    Adaptor adaptor = serialize<Adaptor>(T, key)
    */
template<class Adaptor, typename T>
Adaptor serialize(T&& target, std::string_view key) {
    using origin = detail::remove_cvref_t<T>;
    Adaptor adaptor;
    serde_context<Adaptor, true> ctx(adaptor);
    serde_serializer<origin, serde_context<Adaptor,true>>::into(ctx, target, key);
    return adaptor;
}

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    T -> Adaptor[key] or T -> Adaptor
    serialize_to(T, Adaptor, key)
    */
template<class Adaptor, typename T>
void serialize_to(T&& target, Adaptor& adaptor, std::string_view key) {
    using origin = detail::remove_cvref_t<T>;
    serde_context<Adaptor, true> ctx(adaptor);
    serde_serializer<origin, serde_context<Adaptor, true>>::into(ctx, target, key);
}

// --------------- serde_serializer ---------------
template<typename T, typename serde_ctx, typename flag>
constexpr void serde_serializer<T, serde_ctx, flag>::from(serde_ctx& ctx, T& data, std::string_view key) {
    if constexpr(detail::is_serdeable_v<serde_ctx, T>) {
        if(key.empty()) {
            serde_context struct_ctx = serde_context<Adaptor>{ctx.adaptor};
            data.serde(struct_ctx, data);
        } else {
            serde_adaptor<Adaptor, T, detail::struct_t>::from(ctx.adaptor, key, data);
        }
    } else {
        serde_adaptor<Adaptor, std::remove_reference_t<T>>::from(ctx.adaptor, key, data);
    }
    ctx.read();
}
template<typename T, typename serde_ctx, typename flag>
constexpr void serde_serializer<T, serde_ctx, flag>::into(serde_ctx& ctx, const T& data, std::string_view key) {
    if constexpr(detail::is_serdeable_v<serde_ctx, T>) {
        if(key.empty()) {
            auto struct_ctx = serde_context<Adaptor, true>(ctx.adaptor);
            data.serde(struct_ctx, const_cast<T&>(data));
        } else {
            serde_adaptor<Adaptor, T, detail::struct_t>::into(ctx.adaptor, key, data);
        }
    } else {
        serde_adaptor<Adaptor, std::remove_reference_t<T>>::into(ctx.adaptor, key, data);
    }
    ctx.read();
}

/*
template<class Context, class T, class TUPLE>
constexpr serde_struct<Context, T, TUPLE>::serde_struct(Context& context, T& value) : context_(context), value_(value) {}

template<class Context, class T, class TUPLE>
template<class MEM_PTR, typename Attribute, typename... Attributes>
constexpr auto serde_struct<Context, T, TUPLE>::field(
        MEM_PTR&& ptr, std::string_view name,
        Attribute&& attribute, Attributes&&... attributes)
{
    using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
    using type_tuple = detail::tuple_extend_t<rtype, Tuple>;
    if(context_.skip_all_) {
        return serde_struct<Context, T, type_tuple>(context_, value_);
    }
    if constexpr(!Context::is_serialize) {
        attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                std::forward<detail::remove_cvref_t<Attributes>>
                (const_cast<detail::remove_cvref_t<Attributes>&>(attributes))...,
                attribute::detail::serializer_call_attr{});
    } else {
        attribute.template into<rtype, Context>(context_, value_.*ptr, name,
                std::forward<detail::remove_cvref_t<Attributes>>
                (const_cast<detail::remove_cvref_t<Attributes>&>(attributes))...,
                attribute::detail::serializer_call_attr{});
    }
    return serde_struct<Context, T, type_tuple>(context_, value_);
}

template<class Context, class T, class TUPLE>
template<class MEM_PTR>
constexpr auto serde_struct<Context, T, TUPLE>::field(
        MEM_PTR&& ptr, std::string_view name) 
{
    using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
    using type_tuple = detail::tuple_extend_t<rtype, Tuple>;
    if(context_.skip_all_) {
        return serde_struct<Context, T, type_tuple>(context_, value_);
    }
    if constexpr(!Context::is_serialize) {
        serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
    } else {
        serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
    }
    return serde_struct<Context, T, type_tuple>(context_, value_);
}
    
template<class Context, class T, class TUPLE>
constexpr serde_struct<Context, T, TUPLE>& serde_struct<Context, T, TUPLE>::no_remain() {
    using namespace std::literals;
    if(context_.skip_all_) return *this;
    if constexpr (!Context::is_serialize) {
        const auto adaptor_size = Context::Helper::is_struct(context_.adaptor)
            ? Context::Helper::size(context_.adaptor)
            : 1;
        const auto serde_size = context_.read_count_;
        if(adaptor_size > serde_size) {
            throw unregisted_data_error("serde["s + std::string{type} + "] read: " + std::to_string(serde_size)
                                        + " != adaptor["
                                        + std::string(nameof::nameof_short_type<decltype(context_.adaptor)>())
                                        + "] read: " + std::to_string(adaptor_size));
        }
    }
    return *this;
}

template<class Context, class T, class TUPLE>
template<class MEM_PTR>
constexpr auto serde_struct<Context, T, TUPLE>& serde_struct<Context, T, TUPLE>::operator()(MEM_PTR&& ptr, std::string_view name) {
    using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
    using type_tuple = detail::tuple_extend_t<rtype, Tuple>;
    if(context_.skip_all_) return serde_struct<Context, T, type_tuple>(context_, value_);
    if constexpr(!Context::is_serialize) {
        serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
    } else {
        serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
    }
    return serde_struct<Context, T, type_tuple>(context_, value_);
}

template<class Context, class T, class TUPLE>
template<class MEM_PTR, typename Attribute, typename... Attributes>
constexpr auto serde_struct<Context, T, TUPLE>::operator()(
        MEM_PTR&& ptr, std::string_view name,
        Attribute&& attribute, Attributes&&... attributes) 
{
    using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
    using type_tuple = detail::tuple_extend_t<rtype, Tuple>;
    if(context_.skip_all_) {
        return serde_struct<Context, T, type_tuple>(context_, value_);
    }
    if constexpr(!Context::is_serialize) {
        attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                std::forward<detail::remove_cvref_t<Attributes>>
                (const_cast<detail::remove_cvref_t<Attributes>&>(attributes))...,
                attribute::serializer_call);
    } else {
        attribute.template into<rtype, Context>(context_, value_.*ptr, name,
                std::forward<detail::remove_cvref_t<Attributes>>
                (const_cast<detail::remove_cvref_t<Attributes>&>(attributes))...,
                attribute::serializer_call);
    }
    return serde_struct<Context, T, type_tuple>(context_, value_);
}


template<class Context, class T, class TUPLE>
template<typename Attribute, typename... Attributes>
constexpr serde_struct<Context, T, TUPLE>& serde_struct<Context, T, TUPLE>::attributes(Attribute&& attribute, Attributes&&... attributes) 
{
    if(context_.skip_all_) { return *this; }
    if constexpr(!Context::is_serialize) {
        attribute.template from<T, Context>(context_, value_, "",
                std::forward<detail::remove_cvref_t<Attributes>>
                (const_cast<detail::remove_cvref_t<Attributes>&>(attributes))...,
                attribute::pass);
    } else {
        attribute.template into<T, Context>(context_, value_, "",
                std::forward<detail::remove_cvref_t<Attributes>>
                (const_cast<detail::remove_cvref_t<Attributes>&>(attributes))...,
                attribute::pass);
    }
    return *this;
}

template <typename... Attributes>
struct ApplyAttribute{
    using def = serde_struct<Context, T, Tuple>;
    ApplyAttribute(def& s, std::tuple<Attributes...>&& attr) : s(s), attributes(std::move(attr)) {}
    template<class MEM_PTR>
    inline constexpr def& operator()(MEM_PTR&& ptr, std::string_view name){
        std::apply([&](Attributes&... args){ s(ptr, name, args...);}, attributes);
        return s;
    }
    template <typename... Attributess>
    inline constexpr def& operator[](std::tuple<Attributess...>&& attr) {
        std::apply([&](Attributess&... args){ s.attributes(args...); }, attr);
        return s;
    }
    def& s;
    std::tuple<Attributes...> attributes;
};

template<class Context, class T, class TUPLE>
template <typename... Attributes>
constexpr typename serde_struct<Context, T, TUPLE>::template ApplyAttribute<Attributes...> serde_struct<Context, T, TUPLE>::operator[](std::tuple<Attributes...>&& attr) {
    return ApplyAttribute<Attributes...>(*this, std::move(attr));
}
*/

} // namespace serde

#endif //__SERDEPP_DETAIL_SERIALIZER_DEF_HPP__
