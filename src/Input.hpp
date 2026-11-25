#pragma once

#include "Constants.hpp"
#include <raylib.h>

struct Input{
    Vector2 mouse{};
    Vector2 dir{};
    bool place_held{};
    bool break_held{};
    bool jump_pressed{};
    bool jump_held{};
    bool cycle_pressed{};
};

inline void read_button_input(Input& input, Rectangle window_destination){
    Vector2 mouse = GetMousePosition();
    mouse.x -= window_destination.x;
    mouse.y -= window_destination.y;
    mouse.x /= window_destination.width / WINDOW_WIDTH;
    mouse.y /= window_destination.height / WINDOW_HEIGHT;
    input.mouse = mouse;

    input.dir.x = -IsKeyDown(KEY_A) + IsKeyDown(KEY_D);
    input.dir.y = -IsKeyDown(KEY_W) + IsKeyDown(KEY_S);
    
    if (!input.jump_pressed){
        input.jump_pressed = IsKeyPressed(KEY_SPACE);
    }
    input.jump_held = IsKeyDown(KEY_SPACE);

    if (!input.cycle_pressed){
        input.cycle_pressed = IsKeyPressed(KEY_TAB);
    }

    input.place_held = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    input.break_held = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

inline void consume_presses(Input& input){
    input.jump_pressed = false;
    input.cycle_pressed = false;
}