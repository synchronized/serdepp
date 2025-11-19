#pragma once

#ifndef __CPPSER_ADAPTOR_HPP__
#define __CPPSER_ADAPTOR_HPP__

#include <string>

#include <magic_enum/magic_enum.hpp>
#include <nameof.hpp>

#include "serdepp/exception.hpp"
#include "serdepp/detail/meta.hpp"

namespace serde
{

    template<typename Format, typename=void>
    struct serde_type_checker {
        static bool is_integer(Format& format)  { return true; }
        static bool is_sequence(Format& format) { return true; }
        static bool is_map(Format& format)      { return true; }
        static bool is_float(Format& format)    { return true; }
        static bool is_string(Format& format)   { return true; }
        static bool is_bool(Format& format)     { return true; }
        static bool is_null(Format& format)     { return true; }
        static bool is_struct(Format& format)   { return true; }
    };


    template<typename S, typename T, typename = void>
    struct serde_adaptor {
        static void from(S& s, std::string_view key, T& data);
        static void into(S &s, std::string_view key, const T &data);
    };

    template <class Adaptor, typename=void> 
    struct serde_adaptor_helper {
        // for support optional type parse
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key);
        // for support no_remain function
        inline constexpr static size_t size(Adaptor& adaptor);
        // for support string_or_struct
        inline constexpr static bool is_struct(Adaptor& adaptor);
        // for support parse_file
        inline constexpr static Adaptor parse_file(const std::string& path);
    };

    template <class Adaptor>
    struct derive_serde_adaptor_helper { // default serde adaptor helper
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::is_null(adaptor, key)");
            return false;
        }

        inline constexpr static size_t size(Adaptor& adaptor) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::size(adaptor, key)");
            return 0;
        }

        inline constexpr static bool is_struct(Adaptor& adaptor) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::is_struct(adaptor, key)");
            return false;
        }

        inline constexpr static Adaptor parse_file(const std::string& path) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::parse_file(adaptor, key)");
            return Adaptor{};
        }
    };

    namespace detail {
        struct dummy_adaptor{};
    };

    template <> struct serde_adaptor_helper<detail::dummy_adaptor>
        : derive_serde_adaptor_helper<detail::dummy_adaptor> {
    };

    template<class T, bool is_serialize_=false>
    struct serde_context {
        using Adaptor = T;
        using Helper = serde_adaptor_helper<Adaptor>;
        constexpr static bool is_serialize = is_serialize_;
        constexpr serde_context(T& format) : adaptor(format) {}
        T& adaptor;
        size_t read_count_ = 0;
        bool skip_all_ = false; 
        constexpr void read() { read_count_++; }
    };

    namespace detail {
        struct basic_t{};
        struct poly_t {};
        struct seq_t {};
        struct map_t {};
        struct struct_t {};
        struct enum_t {
            template <class Enum>
            inline constexpr static std::string_view to_str(Enum value) {
                return magic_enum::enum_name(value);
            }

            template <class Enum>
            inline constexpr static Enum from_str(std::string_view str) {
                auto value = magic_enum::enum_cast<Enum>(str);
                if (!value.has_value()) {
                    throw enum_error(std::string{nameof::nameof_type<Enum>()} + "::" + std::string{str});
                }
                return *value;
            }
        };

        template <typename T> using map_e = typename T::mapped_type;
        template <typename T> using map_k = typename T::key_type;
        template <typename T> using seq_e = typename T::value_type;
        template <typename T> using opt_e = typename T::value_type;



        template <class T> using is_struct = detail::is_serdeable<serde_context<detail::dummy_adaptor>, T>;
        template <class T> inline constexpr auto is_struct_v = is_struct<T>::value;
    }
}

#endif //__CPPSER_ADAPTOR_HPP__