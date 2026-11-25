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

inline Rectangle make_spritesheet_source_rectangle(float x, float y){
    return {x * PIXELS_PER_TILE, y * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE};
}

struct Assets{
    Texture spritesheet{};
    Texture background{};

    Sprite player{};
    
    std::unordered_map<TileId, TileSprite> tile_sprites;
};

Assets inline make_assets(){
    Assets assets;

    assets.spritesheet = LoadTexture(ASSETS "spritesheet.png");

    assets.player = Sprite{assets.spritesheet, {48, 48, 16, 16}};

    assets.tile_sprites[TileId::None] = {};
    assets.tile_sprites[TileId::Dirt] = TileSprite{
        .type = TileSpriteType::SpecialTop,
        .tex = assets.spritesheet,
        .normal = make_spritesheet_source_rectangle(1, 3),
        .top = make_spritesheet_source_rectangle(1, 2),
    };
    assets.tile_sprites[TileId::Platform] = TileSprite{
        .type = TileSpriteType::Normal,
        .tex = assets.spritesheet,
        .normal = make_spritesheet_source_rectangle(1, 1),
    };
    assets.tile_sprites[TileId::Spike] = TileSprite{
        .type = TileSpriteType::Attached,
        .tex = assets.spritesheet,
        .normal = make_spritesheet_source_rectangle(1, 0),
        .up = make_spritesheet_source_rectangle(0, 0),
        .down = make_spritesheet_source_rectangle(0, 2),
        .left = make_spritesheet_source_rectangle(0, 3),
        .right = make_spritesheet_source_rectangle(0, 1),
    };
    assets.tile_sprites[TileId::Checkpoint] = TileSprite{
        .type = TileSpriteType::Checkpoint,
        .tex = assets.spritesheet,
        .normal = make_spritesheet_source_rectangle(2, 0),
        .active = make_spritesheet_source_rectangle(2, 1),
    };

    assets.background = LoadTexture(ASSETS "background.png");
    
    return assets;
}