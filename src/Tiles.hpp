#pragma once

#include <cstdint>
#include <array>
#include <variant>
#include <cstdlib>

#include "Utility.hpp"
#include "Assets.hpp"
#include "raylib.h"

enum class TileColision{
    AIR,
    SOLID,
    HAZARD,
    PLATFORM,
};

enum class TileBehaviour{
    NONE,
    CHECKPOINT,
};

using TileSpriteNormal = Sprite;

struct TileSpriteAttached{
    Texture tex{};
    Rectangle up{}, down{}, left{}, right{}, none{};
};

struct TileSpriteSpecialTop{
    Texture tex{};
    Rectangle top{}, normal{};
};

using TileSprite = std::variant<
    TileSpriteNormal,
    TileSpriteAttached,
    TileSpriteSpecialTop
>;

struct TileDef{
    TileSprite sprite;
    TileColision colision{};
    TileBehaviour behaviour{};
};

enum TileId : int16_t{
    NONE = -1,
    DIRT,
    PLATFORM,
    SPIKE,
    CHECKPOINT,

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