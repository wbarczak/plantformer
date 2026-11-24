#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "Global.hpp"
#include "screens/game.hpp"
#include "Constants.hpp"

void calculate_window_info(){
    const float render_width = GetRenderWidth();
    const float render_height = GetRenderHeight();
    const float width_ratio = render_width / WINDOW_WIDTH;
    const float height_ratio = render_height / WINDOW_HEIGHT;

    Rectangle dest{};
    Vector2 mouse_pos = GetMousePosition();

    if (height_ratio < width_ratio){
        dest.width = WINDOW_WIDTH * height_ratio;
        dest.height = render_height;
        dest.x = (render_width - dest.width) / 2;

        mouse_pos.x -= dest.x;
        mouse_pos /= height_ratio;
    }
    else{
        dest.width = render_width;
        dest.height = WINDOW_HEIGHT * width_ratio;
        dest.y = (render_height - dest.height) / 2;

        mouse_pos.y -= dest.y;
        mouse_pos /= width_ratio;
    }

    global.input.mouse = mouse_pos;
    global.render_texture_destination_rectangle = dest;
}

int main(){
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH * DEFAULT_WINDOW_SCALE, WINDOW_HEIGHT * DEFAULT_WINDOW_SCALE, WINDOW_NAME);

    char* db = LoadFileText("gamecontrollerdb.txt");
    if (db) {
        SetGamepadMappings(db);
        UnloadFileText(db);
    }

    RenderTexture2D target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    init_globals();

    game_on_enter();

    float timer{};
    while (!WindowShouldClose()){
        calculate_window_info();
        read_button_input();

        timer += GetFrameTime();
        timer = std::min(timer, .2f);
        while (timer >= DT){
            timer -= DT;
            game_update();
            consume_presses();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginTextureMode(target);
        ClearBackground(DARKGRAY);

        game_draw();

        EndTextureMode();

        DrawTexturePro(
            target.texture, 
            {0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT}, 
            global.render_texture_destination_rectangle,
            {},
            {}, 
            WHITE
        );

        EndDrawing();
    }

    game_on_exit();

    CloseWindow();
}