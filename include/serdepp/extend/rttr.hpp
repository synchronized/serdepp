#pragma once

#include <serdepp/serde.hpp>

#include <rttr/type>

namespace serde {

    struct serde_rttr_type_checker {
        inline static bool is_basic(const rttr::type& value_type) {
            return value_type.is_arithmetic() || value_type.is_enumeration() || value_type == rttr::type::get<std::string>();
        }
    };

    template<typename serde_ctx>
    struct serde_serializer<rttr::variant, serde_ctx, type::basic_t> {
        using ValueType = rttr::variant;
        constexpr inline static auto from(serde_ctx& ctx, ValueType& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, ValueType, type::basic_t>::from(ctx.adaptor, key, data);

            ctx.read();
        }
        constexpr inline static auto into(serde_ctx& ctx, const ValueType& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, ValueType, type::basic_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };

    template<typename serde_ctx>
    struct serde_serializer<rttr::variant, serde_ctx> {
        using ValueType = rttr::variant;
        constexpr inline static auto from(serde_ctx& ctx, ValueType& data, std::string_view key) {
            auto data_type = data.get_type();
            auto wrapped_type = data_type.is_wrapper() ? data_type.get_wrapped_type() : data_type;
            bool is_wrapped = wrapped_type != data_type;
            auto ext_wrapped_value = data.extract_wrapped_value();
            auto& wrapped_value = is_wrapped ? ext_wrapped_value : data;
            auto raw_type = wrapped_type.get_raw_type();

            if (!data || serde_rttr_type_checker::is_basic(raw_type)) 
            {
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant, type::basic_t>::from(ctx.adaptor, key, data);
                ctx.read();
                return;
            }

            else if (raw_type.is_polymoph_container()) 
            {
                auto view = data.create_polymoph_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_polymoph_view, type::poly_t>::from(ctx.adaptor, key, view);
                ctx.read();
                return;
            }

            else if (raw_type.is_sequential_container()) 
            {
                auto view = data.create_sequential_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_sequential_view, type::seq_t>::from(ctx.adaptor, key, view);
                ctx.read();
                return;
            }

            else if (raw_type.is_associative_container()) 
            {
                auto view = data.create_associative_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_associative_view, type::map_t>::from(ctx.adaptor, key, view);
                ctx.read();
                return;
            } 

            else if (raw_type.is_class()) 
            {
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant, type::struct_t>::from(ctx.adaptor, key, data);
                ctx.read();
                return;
            } 

            else if (!data) 
            {
                return;
            }

            throw serde::unimplemented_error("serde_serializer<rttr::variant, serde_ctx>::from()");
        }
        constexpr inline static auto into(serde_ctx& ctx, const ValueType& data, std::string_view key) {
            auto data_type = data.get_type();
            auto wrapped_type = data_type.is_wrapper() ? data_type.get_wrapped_type() : data_type;
            bool is_wrapped = wrapped_type != data_type;
            auto& wrapped_value = is_wrapped ? data.extract_wrapped_value() : data;
            auto raw_type = wrapped_type.get_raw_type();

            if (raw_type.is_arithmetic() || raw_type.is_enumeration() || raw_type == rttr::type::get<std::string>()) 
            {
                serde_serializer<rttr::variant, serde_ctx, type::basic_t>::into(ctx, data, key);
                return;
            }

            else if (raw_type.is_polymoph_container()) 
            {
                auto view = data.create_polymoph_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_polymoph_view, type::poly_t>::into(ctx.adaptor, key, view);
                ctx.read();
                return;

            }

            else if (raw_type.is_sequential_container()) 
            {
                auto view = data.create_sequential_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_sequential_view, type::seq_t>::into(ctx.adaptor, key, view);
                ctx.read();
                return;
            }

            else if (raw_type.is_associative_container()) 
            {
                auto view = data.create_associative_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_associative_view, type::map_t>::into(ctx.adaptor, key, view);
                ctx.read();
                return;
            } 

            else if (raw_type.is_class()) 
            {
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant, type::struct_t>::into(ctx.adaptor, key, data);
                ctx.read();
                return;
            } 

            else if (!data) 
            {
                return;
            }

            throw serde::unimplemented_error("serde_serializer<rttr::variant, serde_ctx>::into()");
        }
    };


}
