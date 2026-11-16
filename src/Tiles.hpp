#pragma once

#include <cstdint>
#include <array>

#include "Utility.hpp"
#include "Assets.hpp"
#include "raylib.h"

enum class TileColision{
    AIR,
    SOLID,
    HAZARD,
    PLATFORM,
};

struct TileSpriteTouchingSurface{
    Texture tex{};
    Rectangle up{}, down{}, left{}, right{}, none{};
};

struct TileSpriteDifferentTop{
    Texture tex{};
    Rectangle top{}, normal{};
};

struct TileSprite{
    enum class Type{
        NORMAL,
        TOUCHING_SURFACE,
        DIFFERENT_TOP,
    };

    union Value{
        Sprite normal;
        TileSpriteTouchingSurface touching_surface;
        TileSpriteDifferentTop different_top;
    };
};

struct TileDef{
    TileColision colision{};
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
    tiles[index(TileId::DIRT)].colision = TileColision::SOLID;
    tiles[index(TileId::PLATFORM)].colision = TileColision::PLATFORM;
    tiles[index(TileId::SPIKE)].colision = TileColision::HAZARD;
}

inline bool valid_tile(TileId id){
    return id != TileId::NONE && id != TileId::COUNT;
}