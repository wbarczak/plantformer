#pragma once

#include <cmath>

#include <raylib.h>

#include "Constants.hpp"

struct Input{
    Vector2 mouse{};
    Vector2 dir{};
    bool right_click{};
    bool left_click{};
    bool place_held{};
    bool break_held{};
    bool toggle_inventory{};
    bool jump_pressed{};
    bool jump_held{};
};

inline void read_button_input(Input& input, Rectangle window_destination){
    Vector2 mouse = GetMousePosition();
    mouse.x -= window_destination.x;
    mouse.y -= window_destination.y;
    mouse.x /= window_destination.width / WINDOW_WIDTH;
    mouse.y /= window_destination.height / WINDOW_HEIGHT;
    input.mouse.x = floorf(mouse.x);
    input.mouse.y = floorf(mouse.y);


    input.right_click = input.right_click || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input.left_click = input.left_click || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    input.toggle_inventory = input.toggle_inventory || IsKeyPressed(KEY_TAB);

    input.dir.x = -IsKeyDown(KEY_A) + IsKeyDown(KEY_D);
    input.dir.y = -IsKeyDown(KEY_W) + IsKeyDown(KEY_S);
    
    input.jump_pressed = input.jump_pressed || IsKeyPressed(KEY_SPACE);
    input.jump_held = IsKeyDown(KEY_SPACE);

    input.place_held = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    input.break_held = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

inline void consume_presses(Input& input){
    input.right_click = false;
    input.left_click = false;
    input.toggle_inventory = false;
    input.jump_pressed = false;
}