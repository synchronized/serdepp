#pragma once

#ifndef __SERDEPP_DETAIL_SERIALIZER_DEF_HPP__
#define __SERDEPP_DETAIL_SERIALIZER_DEF_HPP__

#include <string_view>

namespace serde
{

template<class Adaptor>
constexpr Adaptor parse_file(const std::string& path);

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    Adaptor(key) -> T or Adaptor -> T
    T t = deserialize<T>(Adaptor, key)
    */
template <typename T, class Adaptor>
T deserialize(Adaptor&& adaptor, std::string_view key="");

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    Adaptor[key] -> T or Adaptor -> T
    deserialize_to(Adaptor, T, key)
    */
template<typename T, class Adaptor>
void deserialize_to(Adaptor&& adaptor, T& target, std::string_view key="");

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    T -> Adaptor[key] or T -> Adaptor
    Adaptor adaptor = serialize<Adaptor>(T, key)
    */
template<class Adaptor, typename T>
Adaptor serialize(T&& target, std::string_view key="");

/*
    Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
    T -> Adaptor[key] or T -> Adaptor
    serialize_to(T, Adaptor, key)
    */
template<class Adaptor, typename T>
void serialize_to(T&& target, Adaptor& adaptor, std::string_view key="");

template<typename T, typename serde_ctx, typename = void>
struct serde_serializer {
    using Adaptor = typename serde_ctx::Adaptor;
    constexpr static void from(serde_ctx& ctx, T& data, std::string_view key);
    constexpr static void into(serde_ctx& ctx, const T& data, std::string_view key);
};

/*
template<class Context, class T, class TUPLE=void>
class serde_struct {
    using TYPE = T;
    Context& context_;
    T& value_;
    constexpr const static std::string_view type = nameof::nameof_type<T>();
public:
    using Tuple = TUPLE;
    constexpr serde_struct(Context& context, T& value);

    template<class MEM_PTR>
    constexpr auto field(MEM_PTR&& ptr, std::string_view name);

    template<class MEM_PTR, typename Attribute, typename... Attributes>
    constexpr auto field(MEM_PTR&& ptr, std::string_view name, 
            Attribute&& attribute, Attributes&&... attributes);

    template<class MEM_PTR>
    constexpr auto operator()(MEM_PTR&& ptr, std::string_view name);

    template<class MEM_PTR, typename Attribute, typename... Attributes>
    constexpr auto operator()(MEM_PTR&& ptr, std::string_view name,
            Attribute&& attribute, Attributes&&... attributes);

    constexpr serde_struct& no_remain();

    template<typename Attribute, typename... Attributes>
    constexpr serde_struct& attributes(Attribute&& attribute, 
            Attributes&&... attributes);

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
    constexpr ApplyAttribute<Attributes...> operator[](std::tuple<Attributes...>&& attr);
};
*/

} // namespace serde

#endif //__SERDEPP_DETAIL_SERIALIZER_DEF_HPP__
