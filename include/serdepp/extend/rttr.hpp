#pragma once

#include <rttr/type>

#include "serdepp/detail/adaptor.hpp"
#include "serdepp/detail/serializer_def.hpp"
#include "serdepp/detail/rttr.hpp"

namespace serde {

    template<typename serde_ctx>
    struct serde_serializer<rttr::variant, serde_ctx, detail::basic_t> {
        using ValueType = rttr::variant;
        constexpr inline static auto from(serde_ctx& ctx, ValueType& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, ValueType, detail::basic_t>::from(ctx.adaptor, key, data);

            ctx.read();
        }
        constexpr inline static auto into(serde_ctx& ctx, const ValueType& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, ValueType, detail::basic_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };

    template<typename serde_ctx>
    struct serde_serializer<rttr::variant, serde_ctx> {
        using ValueType = rttr::variant;
        inline static auto from(serde_ctx& ctx, ValueType& data, std::string_view key) {
            auto data_type = data.get_type();
            auto wrapped_type = data_type.is_wrapper() ? data_type.get_wrapped_type() : data_type;
            bool is_wrapped = wrapped_type != data_type;
            auto ext_wrapped_value = data.extract_wrapped_ptr_value();
            auto& wrapped_value = is_wrapped ? ext_wrapped_value : data;
            auto raw_type = wrapped_type.get_raw_type();

            if (!data || detail::rttr_type_is_basic(raw_type)) 
            {
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant, detail::basic_t>::from(ctx.adaptor, key, data);
                ctx.read();
                return;
            }

            else if (raw_type.is_polymoph_container()) 
            {
                auto view = data.create_polymoph_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_polymoph_view, detail::poly_t>::from(ctx.adaptor, key, view);
                ctx.read();
                return;
            }

            else if (raw_type.is_sequential_container()) 
            {
                auto view = data.create_sequential_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_sequential_view, detail::seq_t>::from(ctx.adaptor, key, view);
                ctx.read();
                return;
            }

            else if (raw_type.is_associative_container()) 
            {
                auto view = data.create_associative_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_associative_view, detail::map_t>::from(ctx.adaptor, key, view);
                ctx.read();
                return;
            } 

            else if (raw_type.is_class()) 
            {
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant, detail::struct_t>::from(ctx.adaptor, key, data);
                ctx.read();
                return;
            } 

            else if (!data) 
            {
                return;
            }

            throw serde::unimplemented_error("serde_serializer<rttr::variant, serde_ctx>::from()");
        }
        inline static auto into(serde_ctx& ctx, const ValueType& data, std::string_view key) {
            auto data_type = data.get_type();
            auto wrapped_type = data_type.is_wrapper() ? data_type.get_wrapped_type() : data_type;
            bool is_wrapped = wrapped_type != data_type;
            auto ext_wrapped_value = data.extract_wrapped_ptr_value();
            auto& wrapped_value = is_wrapped ? ext_wrapped_value : data;
            auto raw_type = wrapped_type.get_raw_type();

            if (detail::rttr_type_is_basic(raw_type)) 
            {
                serde_serializer<rttr::variant, serde_ctx, detail::basic_t>::into(ctx, data, key);
                return;
            }

            else if (raw_type.is_polymoph_container()) 
            {
                auto view = data.create_polymoph_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_polymoph_view, detail::poly_t>::into(ctx.adaptor, key, view);
                ctx.read();
                return;

            }

            else if (raw_type.is_sequential_container()) 
            {
                auto view = data.create_sequential_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_sequential_view, detail::seq_t>::into(ctx.adaptor, key, view);
                ctx.read();
                return;
            }

            else if (raw_type.is_associative_container()) 
            {
                auto view = data.create_associative_view();
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant_associative_view, detail::map_t>::into(ctx.adaptor, key, view);
                ctx.read();
                return;
            } 

            else if (raw_type.is_class()) 
            {
                serde_adaptor<typename serde_ctx::Adaptor, rttr::variant, detail::struct_t>::into(ctx.adaptor, key, data);
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
