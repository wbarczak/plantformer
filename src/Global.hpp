#pragma once

#include <array>

#include "Utility.hpp"
#include "Screen.hpp"
#include "Assets.hpp"
#include "Tiles.hpp"

inline struct Global{
    static constexpr int32_t WIDTH = 320;
    static constexpr int32_t HEIGHT = 180;
    const char* WINDOW_NAME = "Platformer";
    static constexpr float SCALE = 4;
    static constexpr float TILE_PIXELS = 16;
    static constexpr int32_t TICKRATE = 60;
    static constexpr float DELTA_TIME = 1.f / TICKRATE;

    Assets assets;
    
    std::array<Screen, index(ScreenId::COUNT)> screens;

    TileArray tile_defs;

    struct{
        float direction_x{};
        bool jump_pressed{};
        bool jump_held{};
    }input;
}g;

inline int32_t find_pad() {
    for (int i = 0; i < 4; i++) {
        if (IsGamepadAvailable(i)) return i;
    }
    return -1;
}

inline void read_input(){
    int32_t pad = find_pad();
    g.input.direction_x = [&]{
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
    if (!g.input.jump_pressed){
        g.input.jump_pressed = IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }
    g.input.jump_held = IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}

inline void disable_presses(){
    g.input.jump_pressed = false;
}