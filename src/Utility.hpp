#pragma once

#include <cstdlib>

template <typename T>
inline constexpr size_t index(T i){
    return static_cast<size_t>(i);
}