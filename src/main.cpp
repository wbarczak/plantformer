#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cmath>

#include <raylib.h>
#include <raymath.h>

struct Sprite{
    Texture tex{};
    Rectangle rect{};
};

enum BlockType{
    A,
    S
};

struct Player{
    Vector2 pos{3, 3};
    Vector2 vel{};
    Rectangle hurtbox{-.25, -.75, .5, .75};
    bool grounded = false;
};

struct Global{
    static constexpr int32_t WIDTH = 320;
    static constexpr int32_t HEIGHT = 180;
    const char* WINDOW_NAME = "Platformer";
    static constexpr float SCALE = 4;
    static constexpr float TILE_PIXELS = 16;
    static constexpr int32_t TICKRATE = 60;
    static constexpr float DELTA_TIME = 1.f / TICKRATE;
    static constexpr float PLAYER_SPEED = 6;
    static constexpr float PLAYER_ACCELERATION = 30;
    static constexpr float PLAYER_DECELERATION = 30;
    static constexpr float GRAVITY_SPEED = 15;
    static constexpr float GRAVITY_ACCELERATION = 20;
    static constexpr float JUMP_ACCELERATION = 11;
    static constexpr float SKIN_WIDTH = 1e-4f;

    struct{
        Texture character;
    }assets;

    struct{
        float direction_x{};
        bool jump_pressed{};
        bool jump_held{};
    }input;

    BlockType map[10][10]{
        {S, S, S, S, S, S, S, S, S, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, A, A, A, A, A, A, A, A, S},
        {S, S, S, S, S, S, S, S, S, S},
    };

    Player player;
}g;

float get_direction(float value){
    return value < 0? -1 : value > 0? 1 : 0;
}

void load_assets(){
    g.assets.character = LoadTexture(ASSETS "sprites/knight.png");
}

int find_pad() {
    for (int i = 0; i < 4; i++) {
        if (IsGamepadAvailable(i)) return i;
    }
    return -1;
}

void read_input(){
    int32_t pad = find_pad();
    g.input.direction_x = [&]{
        float keyboard = -IsKeyDown(KEY_A) + IsKeyDown(KEY_D);

        constexpr static float DEADZONE = .1f;
        float gamepad{};
        float gamepad_x_axis = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        float absolute = std::abs(gamepad_x_axis);
        if (absolute > DEADZONE){
            gamepad = get_direction(gamepad_x_axis) * ((absolute - DEADZONE) / (1.f - DEADZONE));
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

void disable_presses(){
    g.input.jump_pressed = false;
}

void colisions_x(){
    Rectangle starting_hurtbox = g.player.hurtbox;
    starting_hurtbox.x += g.player.pos.x;
    starting_hurtbox.y += g.player.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.x += g.player.vel.x * g.DELTA_TIME;

    if(hurtbox.x < 0){
        hurtbox.x = 0 + g.SKIN_WIDTH;
        g.player.vel.x = 0;
    }
    if (hurtbox.x + hurtbox.width > 10){
        hurtbox.x = 10 - hurtbox.width - g.SKIN_WIDTH;
        g.player.vel.x = 0;
    }
    
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;
    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;
    for (int32_t y = start_y; y <= end_y; y++){
        if (g.map[y][start_x] != A){
            hurtbox.x = start_x + 1 + g.SKIN_WIDTH;
            g.player.vel.x = 0;
            break;
        }
    }
    for (int32_t y = start_y; y <= end_y; y++){
        if (g.map[y][end_x] != A){
            hurtbox.x = end_x - hurtbox.width - g.SKIN_WIDTH;
            g.player.vel.x = 0;
            break;
        }
    }

    g.player.pos.x += hurtbox.x - starting_hurtbox.x;
}

void colisions_y(){
    Rectangle starting_hurtbox = g.player.hurtbox;
    starting_hurtbox.x += g.player.pos.x;
    starting_hurtbox.y += g.player.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.y += g.player.vel.y * g.DELTA_TIME;

    g.player.grounded = false;

    if(hurtbox.y < 0){
        hurtbox.y = 0 + g.SKIN_WIDTH;
        g.player.vel.y = 0;
    }
    if (hurtbox.y + hurtbox.height > 10){
        hurtbox.y = 10 - hurtbox.height - g.SKIN_WIDTH;
        g.player.vel.y = 0;
        g.player.grounded = true;
    }

    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;
    for (int32_t x = start_x; x <= end_x; x++){
        if (g.map[start_y][x] != A){
            hurtbox.y = start_y + 1 + g.SKIN_WIDTH;
            g.player.vel.y = 0;
            break;
        }
    }
    for (int32_t x = start_x; x <= end_x; x++){
        if (g.map[end_y][x] != A){
            hurtbox.y = end_y - hurtbox.height - g.SKIN_WIDTH;
            g.player.vel.y = 0;
            g.player.grounded = true;
            break;
        }
    }

    g.player.pos.y += hurtbox.y - starting_hurtbox.y;
}

void tick(){
    if(g.input.direction_x < 0){
        g.player.vel.x -= g.PLAYER_ACCELERATION * g.DELTA_TIME;
        g.player.vel.x = std::max(g.player.vel.x, -g.PLAYER_SPEED);
    }
    else if(g.input.direction_x > 0){
        g.player.vel.x += g.PLAYER_ACCELERATION * g.DELTA_TIME;
        g.player.vel.x = std::min(g.player.vel.x, g.PLAYER_SPEED);
    }
    else if(g.player.vel.x < 0){
        g.player.vel.x += g.PLAYER_DECELERATION * g.DELTA_TIME;
        g.player.vel.x = std::min(g.player.vel.x, 0.f);
    }
    else if (g.player.vel.x > 0){
        g.player.vel.x -= g.PLAYER_DECELERATION * g.DELTA_TIME;
        g.player.vel.x = std::max(g.player.vel.x, 0.f);
    }
    
    g.player.vel.y += g.GRAVITY_ACCELERATION * g.DELTA_TIME;
    if (g.input.jump_pressed && g.player.grounded){
        g.player.vel.y = -g.JUMP_ACCELERATION;
    }
    else if (g.player.vel.y > 0){
        g.player.vel.y += g.GRAVITY_ACCELERATION * g.DELTA_TIME;
        g.player.vel.y = std::min(g.player.vel.y, g.GRAVITY_SPEED);
    }
    else if(!g.input.jump_held){
        g.player.vel.y = 0;
    }

    colisions_x();
    colisions_y();
}

void draw(){
    for(int32_t y{}; y < 10; y++){
        for(int32_t x{}; x < 10; x++){
            if (g.map[y][x]){
                DrawRectangleGradientEx(
                    {x * g.TILE_PIXELS, y * g.TILE_PIXELS, g.TILE_PIXELS, g.TILE_PIXELS},
                    BLUE,
                    DARKBLUE,
                    DARKBLUE,
                    DARKBLUE
                );
            }
        }
    }

    Rectangle player_hurtbox = g.player.hurtbox;
    player_hurtbox.x += g.player.pos.x;
    player_hurtbox.y += g.player.pos.y;
    player_hurtbox.x *= g.TILE_PIXELS;
    player_hurtbox.y *= g.TILE_PIXELS;
    player_hurtbox.width *= g.TILE_PIXELS;
    player_hurtbox.height *= g.TILE_PIXELS;
    DrawRectanglePro(player_hurtbox, {}, {}, RED);
}

void flip_and_draw_target(RenderTexture2D target){
    const float render_width = GetRenderWidth();
    const float render_height = GetRenderHeight();
    const float width_ratio = render_width / g.WIDTH;
    const float height_ratio = render_height / g.HEIGHT;

    Rectangle dest{};

    if (height_ratio < width_ratio){
        dest.width = g.WIDTH * height_ratio;
        dest.height = render_height;
        dest.x = (render_width - dest.width) / 2;
    }
    else{
        dest.width = render_width;
        dest.height = g.HEIGHT * width_ratio;
        dest.y = (render_height - dest.height) / 2;
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
    load_assets();

    float timer{};

    while (!WindowShouldClose()){
        read_input();

        timer += GetFrameTime();
        timer = std::min(timer, .2f);
        while (timer >= g.DELTA_TIME){
            timer -= g.DELTA_TIME;
            tick();
            disable_presses();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginTextureMode(target);
        ClearBackground(DARKGRAY);

        draw();

        EndTextureMode();

        flip_and_draw_target(target);

        EndDrawing();
    }

    CloseWindow();
}