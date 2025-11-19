#pragma once

namespace serde {
namespace detail {
    inline bool rttr_type_is_basic(const rttr::type& value_type) {
        return value_type.is_arithmetic() || value_type.is_enumeration() || value_type == rttr::type::get<std::string>();
    }
}
}
