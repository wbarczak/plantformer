#pragma once

#include <cstdint>

template <typename T>
inline constexpr int32_t index(T i){
    return static_cast<int32_t>(i);
}