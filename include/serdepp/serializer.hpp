#pragma once

#ifndef __CPPSER_SERIALIZER_HPP__
#define __CPPSER_SERIALIZER_HPP__

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <typeinfo>
#include <set>
#include <list>
#include <type_traits>
#include <variant>
#include <string_view>
#include <functional>
#include <algorithm>

#include "serdepp/detail/adaptor.hpp"
#include "serdepp/detail/serializer_def.hpp"

namespace serde
{

    namespace attribute {
        namespace detail {
            struct serializer_call_attr {
                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                void from(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    serde::serde_serializer<T, serde_ctx>::from(ctx, data, key);
                    next_attr.template from<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                void into(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    serde::serde_serializer<T, serde_ctx>::into(ctx, data, key);
                    next_attr.template into<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx>
                void from(serde_ctx& ctx, T& data, std::string_view key) const {
                    serde::serde_serializer<T, serde_ctx>::from(ctx, data, key);
                }

                template<typename T, typename serde_ctx>
                void into(serde_ctx& ctx, T& data, std::string_view key) const { 
                    serde::serde_serializer<T, serde_ctx>::into(ctx, data, key);
                }
            };

            struct empty_attr {
                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                constexpr void from(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    next_attr.template from<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                constexpr void into(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    next_attr.template into<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx>
                constexpr void from(serde_ctx& ctx, T& data, std::string_view key) const { }

                template<typename T, typename serde_ctx>
                constexpr void into(serde_ctx& ctx, T& data, std::string_view key) const { }
            };
        }
        constexpr auto serializer_call = detail::serializer_call_attr{};
        constexpr auto pass = detail::empty_attr{};

    }

    template<class Context, class T, class TUPLE=void>
    class serde_struct {
        using TYPE = T;
        Context& context_;
        T& value_;
        constexpr const static std::string_view type = nameof::nameof_type<T>();
    public:
        using Tuple = TUPLE;
        constexpr serde_struct(Context& context, T& value) 
                : context_(context), value_(value) {}

        template<class MEM_PTR, typename Attribute, typename... Attributes>
        constexpr auto field(MEM_PTR&& ptr, std::string_view name,
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

        template<class MEM_PTR>
        constexpr auto field(MEM_PTR&& ptr, std::string_view name) 
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
        
        constexpr serde_struct& no_remain() {
            using namespace std::literals;
            if(context_.skip_all_) { return *this; }
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


        template<class MEM_PTR>
        constexpr auto operator()(MEM_PTR&& ptr, std::string_view name) {
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

        template<class MEM_PTR, typename Attribute, typename... Attributes>
        constexpr auto operator()(MEM_PTR&& ptr, std::string_view name,
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

        template<typename Attribute, typename... Attributes>
        constexpr serde_struct& attributes(Attribute&& attribute, Attributes&&... attributes) {
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
            constexpr def& operator()(MEM_PTR&& ptr, std::string_view name){
                std::apply([&](Attributes&... args){ s(ptr, name, args...);}, attributes);
                return s;
            }
            template <typename... Attributess>
            constexpr def& operator[](std::tuple<Attributess...>&& attr) {
                std::apply([&](Attributess&... args){ s.attributes(args...); }, attr);
                return s;
            }
            def& s;
            std::tuple<Attributes...> attributes;
        };

        template <typename... Attributes>
        constexpr ApplyAttribute<Attributes...> operator[](std::tuple<Attributes...>&& attr) {
            return ApplyAttribute<Attributes...>(*this, std::move(attr));
        }
    };

    namespace attribute {
        template<typename... Ty>
        constexpr std::tuple<Ty...> attributes(Ty&&... arg) {
            return std::make_tuple(std::forward<Ty>(arg)...);
        }

        template<class Context, class T, typename... Ty>
        constexpr serde::serde_struct<Context, T> operator|(std::tuple<Ty...> attributes,
                serde::serde_struct<Context, T> serde_define) 
        {
            auto ptsr = [&](auto... v) {serde_define.attr(v...);};
            std::apply(ptsr, std::move(attributes));
            return std::move(serde_define);
        }
    }

    template<class Context, class T> 
    serde_struct(Context&, T&) -> serde_struct<Context, T>;

    template<typename serde_ctx>
    struct serde_serializer<std::monostate, serde_ctx> {
        constexpr static auto from(serde_ctx& ctx, std::monostate& data, std::string_view key) {}
        constexpr static auto into(serde_ctx& ctx, const std::monostate& data, std::string_view key) {}
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<detail::is_mappable_v<T> &&
                                                           detail::is_emptyable_v<T>>> {
        constexpr static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, detail::map_t>::from(ctx.adaptor, key, data);
            ctx.read();
        }
        constexpr static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, detail::map_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<detail::is_sequenceable_v<T> &&
                                                           detail::is_emptyable_v<T> &&
                                                           !detail::is_str_v<T>>>{
        constexpr static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, detail::seq_t>::from(ctx.adaptor, key, data);
            ctx.read();
        }

        constexpr static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, detail::seq_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };


    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<detail::is_enumable_v<T>>>{
        static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            std::string str_to_enum;
            serde_adaptor<typename serde_ctx::Adaptor, std::string>::from(ctx.adaptor, key, str_to_enum);
            data = detail::enum_t::from_str<T>(str_to_enum);
            ctx.read();
        }
        static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            auto enum_to_str = std::string{detail::enum_t::to_str(data)};
            serde_adaptor<typename serde_ctx::Adaptor, std::string>::into(ctx.adaptor, key, enum_to_str);
            ctx.read();
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<detail::is_optional_v<T>>> {
        using value_type = detail::opt_e<std::remove_reference_t<T>>;
        using Adaptor = typename serde_ctx::Adaptor;
        using Helper = serde_adaptor_helper<Adaptor>;
        constexpr static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if(key.empty() && serde_type_checker<Adaptor>::is_null(ctx.adaptor)) {
                if(!data) data = std::nullopt;
            } else if (!key.empty() && Helper::is_null(ctx.adaptor, key)) {
                if(!data) data = std::nullopt;
            } else {
                if(!data) data.emplace();
                serde_serializer<value_type, serde_ctx>::from(ctx, *data, key);
          }
        }
        constexpr static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            if(data) { serde_serializer<value_type, serde_ctx>::into(ctx, *data, key); }
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<detail::is_pointer_v<T>>> {
        using Adaptor = typename serde_ctx::Adaptor;
        using Helper = serde_adaptor_helper<Adaptor>;
        using ptr_helper = detail::is_pointer<T>;
        constexpr static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if(key.empty() && serde_type_checker<Adaptor>::is_null(ctx.adaptor)) {
                if(!data) data = ptr_helper::null_type;
            }
            else if(!key.empty() && Helper::is_null(ctx.adaptor, key)) {
                if(!data) data = ptr_helper::null_type;
            } else {
                if(!data) data = ptr_helper::init();
                serde_serializer<typename ptr_helper::type, serde_ctx>::from(ctx, *data, key);
            }
        }
        constexpr static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            if(data) { 
                serde_serializer<typename ptr_helper::type, serde_ctx>::into(ctx, *data, key); 
            }
        }
    };

    template<typename T, class Context=serde_context<detail::dummy_adaptor, true>, typename=void>
    struct to_tuple {
        using origin = detail::remove_cvref_t<T>;
        using type = T;
    };

    template<typename T>
    struct to_tuple<T, serde_context<detail::dummy_adaptor, true>, std::enable_if_t<detail::is_struct_v<T>>>{
        using origin = detail::remove_cvref_t<T>;
        using Context = serde_context<detail::dummy_adaptor, true>;
        using type = typename decltype(std::declval<T>().template
                                    /*auto*/ serde<Context>(/*Context& ctx)*/
                                        std::add_lvalue_reference_t<Context>(std::declval<Context>()), /*format& */
                                        std::add_lvalue_reference_t<T>(std::declval<T>()) /*value& */))::Tuple;

    };
    /*compile time: stuct type -> tuple type*/
    template<class T> 
    using to_tuple_t = typename to_tuple<T>::type;

    template<class T> 
    struct tuple_size;

    template<class T> 
    struct tuple_size :
        public std::integral_constant<std::size_t, std::tuple_size_v<serde::to_tuple_t<T>>> {};

    /*compile time: stuct member size*/
    template<class T> 
    [[maybe_unused]] constexpr static size_t tuple_size_v = tuple_size<T>::value;

} // namespace serde

#include "serdepp/detail/impl/serializer_impl.hpp"

#endif
