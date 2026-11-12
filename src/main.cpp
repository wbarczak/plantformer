#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "Global.hpp"
#include "screens/game.hpp"

void flip_and_draw_target(RenderTexture2D target){
    const float render_width = GetRenderWidth();
    const float render_height = GetRenderHeight();
    const float width_ratio = render_width / g.WIDTH;
    const float height_ratio = render_height / g.HEIGHT;

    Rectangle dest{};
    // this mouse stuff should be done at the beginning of the frame
    g.input.mouse = GetMousePosition();

    if (height_ratio < width_ratio){
        dest.width = g.WIDTH * height_ratio;
        dest.height = render_height;
        dest.x = (render_width - dest.width) / 2;

        g.input.mouse.x -= dest.x;
        g.input.mouse /= height_ratio;
    }
    else{
        dest.width = render_width;
        dest.height = g.HEIGHT * width_ratio;
        dest.y = (render_height - dest.height) / 2;

        g.input.mouse.y -= dest.y;
        g.input.mouse /= width_ratio;
    }

    DrawTexturePro(
        target.texture, 
        {0, 0, g.WIDTH, -g.HEIGHT}, 
        dest,
        {},
        {}, 
        WHITE
    );
}

int main(){
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(g.WIDTH * g.SCALE, g.HEIGHT * g.SCALE, g.WINDOW_NAME);

    char* db = LoadFileText("gamecontrollerdb.txt");
    if (db) {
        SetGamepadMappings(db);
        UnloadFileText(db);
    }
    /*SetGamepadMappings(
        "03000000c82d00000a31000014010000,8BitDo Ultimate 2C Wireless Controller,"
        "platform:Linux,"
        "a:b0,b:b1,x:b2,y:b3,"
        "leftshoulder:b4,rightshoulder:b5,"
        "back:b6,start:b7,guide:b8,"
        "leftstick:b9,rightstick:b10,"
        "dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,"
        "leftx:a0,lefty:a1,rightx:a3,righty:a4,"
        "lefttrigger:a2,righttrigger:a5,");*/

    RenderTexture2D target = LoadRenderTexture(g.WIDTH, g.HEIGHT);
    init_assets(g.assets);
    init_tiles(g.tile_defs);

    game_on_enter();

    float timer{};
    while (!WindowShouldClose()){
        read_input();

        timer += GetFrameTime();
        timer = std::min(timer, .2f);
        while (timer >= g.DELTA_TIME){
            timer -= g.DELTA_TIME;
            game_update();
            disable_presses();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginTextureMode(target);
        ClearBackground(DARKGRAY);

        game_draw();

        EndTextureMode();

        flip_and_draw_target(target);

        EndDrawing();
    }

    game_on_exit();

    CloseWindow();
}