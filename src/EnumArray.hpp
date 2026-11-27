#pragma once

#include <cstdlib>
#include <array>

template <typename ENUM, typename T, size_t N>
struct EnumArray{
    std::array<T, N> data;

    constexpr T& operator[](ENUM e){
        return data[static_cast<size_t>(e)];
    }

    constexpr const T& operator[](ENUM e) const {
        return data[static_cast<size_t>(e)];
    }
};