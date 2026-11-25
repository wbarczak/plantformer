#pragma once

#include <unordered_map>
#include <array>

#include "Screen.hpp"
#include "Assets.hpp"
#include "Tiles.hpp"

inline struct Global{
    Assets assets;
    
    std::array<Screen, SCREEN_COUNT> screens;

    std::unordered_map<TileId, TileDef> tiles;

    struct{
        Vector2 mouse{};
        float direction_x{};
        bool jump_pressed{};
        bool jump_held{};
    }input;

    Rectangle render_texture_destination_rectangle{};
}global;

inline void init_globals(){
    auto& tiles = global.tiles;

    tiles[TileId::None] = {};
    tiles[TileId::Dirt].colision = TileColision::Solid;
    tiles[TileId::Platform].colision = TileColision::Platform;
    tiles[TileId::Spike].colision = TileColision::Hazard;
    tiles[TileId::Checkpoint].behaviour = TileBehaviour::Checkpoint;

    auto& assets = global.assets;

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
}

inline int32_t find_pad() {
    for (int i = 0; i < 4; i++) {
        if (IsGamepadAvailable(i)) return i;
    }
    return -1;
}

inline void read_button_input(){
    auto& input = global.input;

    int32_t pad = find_pad();
    input.direction_x = [&]{
        float keyboard = -IsKeyDown(KEY_A) + IsKeyDown(KEY_D);

        constexpr static float DEADZONE = .1f;
        float gamepad{};
        float gamepad_x_axis = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        float absolute = std::abs(gamepad_x_axis);
        if (absolute > DEADZONE){
            gamepad = (gamepad_x_axis < 0? -1 : gamepad_x_axis > 0? 1 : 0) * ((absolute - DEADZONE) / (1.f - DEADZONE));
        }

        if (keyboard){
            return keyboard;
        }
        if (gamepad){
            return gamepad;
        }
        return 0.f;
    }();
    if (!input.jump_pressed){
        input.jump_pressed = IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }
    input.jump_held = IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}

inline void consume_presses(){
    global.input.jump_pressed = false;
}