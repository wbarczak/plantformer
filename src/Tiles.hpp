#pragma once

#include <cstdint>
#include <array>

#include "Utility.hpp"

enum class TileType{
    AIR,
    SOLID,
    HAZARD,
    PLATFORM,
};

struct TileDef{
    TileType type{};
};

enum TileId : int16_t{
    NONE = -1,
    DIRT,
    PLATFORM,
    SPIKE,

    COUNT,
};

using TileArray = std::array<TileDef, static_cast<size_t>(TileId::COUNT)>;

inline void init_tiles(TileArray& tiles){
    tiles[index(TileId::DIRT)].type = TileType::SOLID;
    tiles[index(TileId::PLATFORM)].type = TileType::PLATFORM;
    tiles[index(TileId::SPIKE)].type = TileType::HAZARD;
}

inline bool valid_tile(TileId id){
    return id != TileId::NONE && id != TileId::COUNT;
}