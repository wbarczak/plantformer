#pragma once

#include <unordered_map>
#include <cstdint>

#include <raylib.h>

#include "Constants.hpp"
#include "Tiles.hpp"
#include "Items.hpp"

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

Rectangle inline make_spritesheet_source_rectangle(float x, float y){
    return {x * PIXELS_PER_TILE, y * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE};
}

Rectangle inline make_mini_spritesheet_source_rectangle(float x, float y){
    return {x * PIXELS_PER_MINI_ICON, y * PIXELS_PER_MINI_ICON, PIXELS_PER_MINI_ICON, PIXELS_PER_MINI_ICON};
}

struct Assets{
    Texture spritesheet{};
    Texture mini_icon_spritesheet{};

    Sprite player{};
    
    std::unordered_map<TileId, TileSprite> tile_sprites;
    std::unordered_map<ItemId, Sprite> item_sprites;
    std::unordered_map<ItemId, Sprite> mini_icon_sprites;
};

Assets inline make_assets(){
    Assets assets;

    assets.spritesheet = LoadTexture(ASSETS "spritesheet.png");

    assets.player = Sprite{
        .tex = assets.spritesheet,
        .rect = make_spritesheet_source_rectangle(3, 3),
    };

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

    assets.item_sprites[ItemId::Air] = {};
    assets.item_sprites[ItemId::TileDirt] = Sprite{
        .tex = assets.spritesheet,
        .rect = make_spritesheet_source_rectangle(1, 2)
    };
    assets.item_sprites[ItemId::TilePlatform] = Sprite{
        .tex = assets.spritesheet,
        .rect = make_spritesheet_source_rectangle(1, 1)
    };
    assets.item_sprites[ItemId::TileSpike] = Sprite{
        .tex = assets.spritesheet,
        .rect = make_spritesheet_source_rectangle(0, 0)
    };
    assets.item_sprites[ItemId::TileCheckpoint] = Sprite{
        .tex = assets.spritesheet,
        .rect = make_spritesheet_source_rectangle(2, 1)
    };

    assets.mini_icon_spritesheet = LoadTexture("mini_icons.png");
    assets.mini_icon_sprites[ItemId::TileDirt] = Sprite{
        .tex = assets.mini_icon_spritesheet,
        .rect = make_mini_spritesheet_source_rectangle(2, 0)
    };
    assets.mini_icon_sprites[ItemId::TilePlatform] = Sprite{
        .tex = assets.mini_icon_spritesheet,
        .rect = make_mini_spritesheet_source_rectangle(1, 0)
    };
    assets.mini_icon_sprites[ItemId::TileSpike] = Sprite{
        .tex = assets.mini_icon_spritesheet,
        .rect = make_mini_spritesheet_source_rectangle(0, 0)
    };
    assets.mini_icon_sprites[ItemId::TileCheckpoint] = Sprite{
        .tex = assets.mini_icon_spritesheet,
        .rect = make_mini_spritesheet_source_rectangle(3, 0)
    };
    
    return assets;
}