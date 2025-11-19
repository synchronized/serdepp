#pragma once

#ifndef __SERDEPP_DETAIL_SERIALIZER_DEF_HPP__
#define __SERDEPP_DETAIL_SERIALIZER_DEF_HPP__

#include <string_view>

namespace serde
{

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      Adaptor(key) -> T or Adaptor -> T
      T t = deserialize<T>(Adaptor, key)
     */
    template <typename T, class Adaptor>
    inline T deserialize(Adaptor&& adaptor, std::string_view key="");

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      Adaptor[key] -> T or Adaptor -> T
      deserialize_to(Adaptor, T, key)
     */
    template<typename T, class Adaptor>
    inline void deserialize_to(Adaptor&& adaptor, T& target, std::string_view key="");

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      T -> Adaptor[key] or T -> Adaptor
      Adaptor adaptor = serialize<Adaptor>(T, key)
     */
    template<class Adaptor, typename T>
    inline Adaptor serialize(T&& target, std::string_view key="");

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      T -> Adaptor[key] or T -> Adaptor
      serialize_to(T, Adaptor, key)
     */
    template<class Adaptor, typename T>
    inline void serialize_to(T&& target, Adaptor& adaptor, std::string_view key="");

    template<typename T, typename serde_ctx, typename = void>
    struct serde_serializer {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
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
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
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
    };

} // namespace serde

#endif //__SERDEPP_DETAIL_SERIALIZER_DEF_HPP__
