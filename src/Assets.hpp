#pragma once

#include <raylib.h>

struct Sprite{
    Texture tex{};
    Rectangle rect{};
};

struct Assets{
    Texture spritesheet{};
    
    struct{
        Sprite grass;
        Sprite dirt;
        Sprite platform;
        Sprite spike;
        Sprite spike_up;
        Sprite spike_down;
        Sprite spike_left;
        Sprite spike_right;

        Sprite player;
    }sprites;
};

inline void init_assets(Assets& assets){
    assets.spritesheet = LoadTexture(ASSETS "spritesheet.png");

    assets.sprites.player = Sprite{assets.spritesheet, {48, 48, 16, 16}};
    assets.sprites.grass = Sprite{assets.spritesheet, {16, 32, 16, 16}};
    assets.sprites.dirt = Sprite{assets.spritesheet, {16, 48, 16, 16}};
    assets.sprites.platform = Sprite{assets.spritesheet, {16, 16, 16, 16}};
    assets.sprites.spike = Sprite{assets.spritesheet, {16, 0, 16, 16}};
    assets.sprites.spike_up = Sprite{assets.spritesheet, {0, 0, 16, 16}};
    assets.sprites.spike_down = Sprite{assets.spritesheet, {0, 32, 16, 16}};
    assets.sprites.spike_left = Sprite{assets.spritesheet, {0, 48, 16, 16}};
    assets.sprites.spike_right = Sprite{assets.spritesheet, {0, 16, 16, 16}};
}