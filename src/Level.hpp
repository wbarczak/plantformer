#pragma once

#include <array>
#include <cstdint>

#include "Tiles.hpp"

struct Level{
    static constexpr int32_t WIDTH = 100;
    static constexpr int32_t HEIGHT = 100;
    std::array<TileId, WIDTH * HEIGHT> flat;
};

inline TileId& get_tile(Level& level, int32_t x, int32_t y){
    return level.flat[y * Level::WIDTH + x];
}