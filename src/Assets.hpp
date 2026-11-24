#pragma once

#include <unordered_map>
#include <cstdint>

#include <raylib.h>

#include "Constants.hpp"
#include "Tiles.hpp"

struct Sprite{
    Texture tex{};
    Rectangle rect{};
};

using TileSpriteNormal = Sprite;

enum class TileSpriteType : uint8_t{
    Normal,
    SpecialTop,
    Attached,
    Checkpoint,
};

struct TileSprite{
    TileSpriteType type{};
    Texture tex{};
    Rectangle normal{}, top{}, active{}, up{}, down{}, left{}, right{};
};

inline Rectangle make_spritesheet_origin_rectangle(float x, float y){
    return {x * PIXELS_PER_TILE, y * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE};
}

struct Assets{
    Texture spritesheet{};
    Texture background{};

    Sprite player{};
    
    std::unordered_map<TileId, TileSprite> tile_sprites;
};