#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <exception>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "Assets.hpp"
#include "Global.hpp"
#include "Constants.hpp"

Rectangle inline calculate_window_destination(){
    const float render_width = GetRenderWidth();
    const float render_height = GetRenderHeight();
    const float width_ratio = render_width / WINDOW_WIDTH;
    const float height_ratio = render_height / WINDOW_HEIGHT;

    Rectangle dest{};

    if (height_ratio < width_ratio){
        dest.width = WINDOW_WIDTH * height_ratio;
        dest.height = render_height;
        dest.x = (render_width - dest.width) / 2;
    }
    else{
        dest.width = render_width;
        dest.height = WINDOW_HEIGHT * width_ratio;
        dest.y = (render_height - dest.height) / 2;
    }

    return dest;
}

int main(){
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH * DEFAULT_WINDOW_SCALE, WINDOW_HEIGHT * DEFAULT_WINDOW_SCALE, WINDOW_NAME);

    // char* db = LoadFileText("gamecontrollerdb.txt");
    // if (db) {
    //     SetGamepadMappings(db);
    //     UnloadFileText(db);
    // }

    init_global();
    ScreenId active_screen_id = global.active_screen_id;
    const Screen* active_screen = &global.screens.at(active_screen_id);
    active_screen->on_enter();

    RenderTexture2D target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

    float timer{};
    while (!WindowShouldClose()){
        global.render_texture_destination_rectangle = calculate_window_destination();
        read_button_input(global.input, global.render_texture_destination_rectangle);

        // Temporary hot reloading of assets, it does not deallocate the old assets
        if(IsKeyPressed(KEY_R)){
            global.assets = make_assets();
        }

        global.elapsed += GetFrameTime();

        timer += GetFrameTime();
        timer = std::min(timer, .2f);
        while (timer >= DT){
            timer -= DT;
            active_screen->update();
            consume_presses(global.input);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginTextureMode(target);
        ClearBackground(DARKGRAY);

        active_screen->draw();

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

        if(active_screen_id != global.active_screen_id){
            active_screen->on_exit();
            active_screen = &global.screens.at(global.active_screen_id);
            active_screen->on_enter();
        }
    }

    active_screen->on_exit();

    CloseWindow();
}