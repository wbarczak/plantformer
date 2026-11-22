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
    const float width_ratio = render_width / k::window_width;
    const float height_ratio = render_height / k::window_height;

    Rectangle dest{};
    Vector2 mouse_pos = GetMousePosition();

    if (height_ratio < width_ratio){
        dest.width = k::window_width * height_ratio;
        dest.height = render_height;
        dest.x = (render_width - dest.width) / 2;

        mouse_pos.x -= dest.x;
        mouse_pos /= height_ratio;
    }
    else{
        dest.width = render_width;
        dest.height = k::window_height * width_ratio;
        dest.y = (render_height - dest.height) / 2;

        mouse_pos.y -= dest.y;
        mouse_pos /= width_ratio;
    }

    g.input.mouse = mouse_pos;
    g.render_texture_destination_rectangle = dest;
}

int main(){
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow( k::window_width * k::default_window_scale, k::window_height * k::default_window_scale, k::window_name);

    char* db = LoadFileText("gamecontrollerdb.txt");
    if (db) {
        SetGamepadMappings(db);
        UnloadFileText(db);
    }

    RenderTexture2D target = LoadRenderTexture(k::window_width, k::window_height);
    init_assets(g.assets);
    init_tiles(g.tile_defs);

    game_on_enter();

    float timer{};
    while (!WindowShouldClose()){
        calculate_window_info();
        read_button_input();

        timer += GetFrameTime();
        timer = std::min(timer, .2f);
        while (timer >= k::delta_time){
            timer -= k::delta_time;
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
            {0, 0, k::window_width, -k::window_height}, 
            g.render_texture_destination_rectangle,
            {},
            {}, 
            WHITE
        );

        EndDrawing();
    }

    game_on_exit();

    CloseWindow();
}