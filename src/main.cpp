#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <unordered_set>

#include <raylib.h>
#include <raymath.h>

struct Sprite{
    Texture tex{};
    Rectangle rect{};
};

enum class Tile{
    AIR = -1,
    SPIKE_UP = 0,
    SPIKE = 1,
    SPIKE_RIGHT = 4,
    PLATFORM = 5,
    SPIKE_DOWN = 8,
    GRASS = 9,
    SPIKE_LEFT = 12,
    DIRT = 13,
};

struct Player{
    Vector2 pos{3, 3};
    Vector2 vel{};
    Rectangle hurtbox{-.25, -.75, .5, .75};
    bool grounded = false;
    bool facing_left = false;
};

struct Level{
    std::vector<Tile> flat;
    int32_t width{};
    int32_t height{};
};

Level load_level(const char* path){
    Level out;
    
    std::ifstream file(path);
    std::string buffer;
    bool first_row = true;
    while(file >> buffer){
        size_t last_comma = -1;
        int32_t id{};
        for(size_t i{}; i < buffer.size(); i++){
            if (buffer[i] == ',' || i == buffer.size() - 1){
                id = atoi(buffer.substr(last_comma + 1, i - last_comma).c_str());
                last_comma = i;
                out.flat.push_back(static_cast<Tile>(id));
            }
        }

        if(first_row){
            first_row = false;
            out.width = out.flat.size();
        }
    }
    
    out.height = out.flat.size() / out.width;
    return out;
}

Tile& get_tile(Level& level, int32_t x, int32_t y){
    return level.flat[y * level.width + x];
}

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

    Texture spritesheet;
    
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

    struct{
        float direction_x{};
        bool jump_pressed{};
        bool jump_held{};
    }input;

    struct{
        std::unordered_set<Tile> platform{
            Tile::PLATFORM,
        };

        std::unordered_set<Tile> hazard{
            Tile::SPIKE,
            Tile::SPIKE_DOWN,
            Tile::SPIKE_UP,
            Tile::SPIKE_LEFT,
            Tile::SPIKE_RIGHT,
        };
    }tile_sets;

    Player player;
    Level level;
    Camera2D cam{.offset{static_cast<float>(WIDTH) / 2, static_cast<float>(HEIGHT) / 2}, .zoom = 1};
}g;

float get_direction(float value){
    return value < 0? -1 : value > 0? 1 : 0;
}

void load_assets(){
    g.spritesheet = LoadTexture(ASSETS "spritesheet.png");

    g.sprites.player = Sprite{g.spritesheet, {48, 48, 16, 16}};
    g.sprites.grass = Sprite{g.spritesheet, {16, 32, 16, 16}};
    g.sprites.dirt = Sprite{g.spritesheet, {16, 48, 16, 16}};
    g.sprites.platform = Sprite{g.spritesheet, {16, 16, 16, 16}};
    g.sprites.spike = Sprite{g.spritesheet, {16, 0, 16, 16}};
    g.sprites.spike_up = Sprite{g.spritesheet, {0, 0, 16, 16}};
    g.sprites.spike_down = Sprite{g.spritesheet, {0, 32, 16, 16}};
    g.sprites.spike_left = Sprite{g.spritesheet, {0, 48, 16, 16}};
    g.sprites.spike_right = Sprite{g.spritesheet, {0, 16, 16, 16}};
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
    if (hurtbox.x + hurtbox.width > g.level.width){
        hurtbox.x = g.level.width - hurtbox.width - g.SKIN_WIDTH;
        g.player.vel.x = 0;
    }
    
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;
    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;

    for (int32_t y = start_y; y <= end_y; y++){
        Tile tile = get_tile(g.level, start_x, y);

        if(tile == Tile::AIR || g.tile_sets.platform.contains(tile)){
            continue;
        }
        else if(g.tile_sets.hazard.contains(tile)){

        }
        else{
            hurtbox.x = start_x + 1 + g.SKIN_WIDTH;
            g.player.vel.x = 0;
            break;
        }
    }

    start_x = hurtbox.x;
    end_x = hurtbox.x + hurtbox.width;

    for (int32_t y = start_y; y <= end_y; y++){
        Tile tile = get_tile(g.level, end_x, y);

        if(tile == Tile::AIR || g.tile_sets.platform.contains(tile)){
            continue;
        }
        else if(g.tile_sets.hazard.contains(tile)){

        }
        else{
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
        //g.player.vel.y = 0;
    }
    if (hurtbox.y + hurtbox.height > g.level.height){
        hurtbox.y = g.level.height - hurtbox.height - g.SKIN_WIDTH;
        g.player.vel.y = 0;
        g.player.grounded = true;
    }

    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;

    for (int32_t x = start_x; x <= end_x; x++){
        Tile tile = get_tile(g.level, x, start_y);

        if(tile == Tile::AIR || g.tile_sets.platform.contains(tile)){
            continue;
        }
        else if(g.tile_sets.hazard.contains(tile)){

        }
        else{
            hurtbox.y = start_y + 1 + g.SKIN_WIDTH;
            //g.player.vel.y = 0;
            break;
        }
    }

    start_y = hurtbox.y;
    end_y = hurtbox.y + hurtbox.height;

    for (int32_t x = start_x; x <= end_x; x++){
        Tile tile = get_tile(g.level, x, end_y);
        bool is_platform = g.tile_sets.platform.contains(tile);

        if(tile == Tile::AIR){
            continue;
        }
        else if(g.tile_sets.hazard.contains(tile)){

        }
        else if((is_platform && floor(hurtbox.y + hurtbox.height) > floor(starting_hurtbox.y + starting_hurtbox.height)) || !is_platform){
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

    if(g.player.vel.x != 0){
        g.player.facing_left = g.player.vel.x < 0;
    }

    colisions_x();
    colisions_y();
}

void draw(){
    g.cam.target = g.player.pos * g.TILE_PIXELS;
    BeginMode2D(g.cam);

    for(int32_t y{}; y < g.level.height; y++){
        for(int32_t x{}; x < g.level.width; x++){
            Tile tile = get_tile(g.level, x, y);
            Sprite sprite{};
            switch (tile){
                case Tile::AIR: break;
                case Tile::DIRT: sprite = g.sprites.dirt; break;
                case Tile::GRASS: sprite = g.sprites.grass; break;
                case Tile::PLATFORM: sprite = g.sprites.platform; break;
                case Tile::SPIKE: sprite = g.sprites.spike; break;
                case Tile::SPIKE_DOWN: sprite = g.sprites.spike_down; break;
                case Tile::SPIKE_UP: sprite = g.sprites.spike_up; break;
                case Tile::SPIKE_LEFT: sprite = g.sprites.spike_left; break;
                case Tile::SPIKE_RIGHT: sprite = g.sprites.spike_right; break;
            }
            DrawTexturePro(sprite.tex, sprite.rect, {x * g.TILE_PIXELS, y * g.TILE_PIXELS, 16, 16}, {}, {}, WHITE);
        }
    }

    Rectangle player_rect = g.sprites.player.rect;
    if (g.player.facing_left){
        player_rect.width *= -1;
    }

    DrawTexturePro(
        g.sprites.player.tex,
        player_rect,
        {(g.player.pos.x - .5f) * g.TILE_PIXELS, (g.player.pos.y - 1) * g.TILE_PIXELS, 16, 16},
        {},
        {},
        WHITE
    );

    Rectangle player_hurtbox = g.player.hurtbox;
    player_hurtbox.x += g.player.pos.x;
    player_hurtbox.y += g.player.pos.y;
    player_hurtbox.x *= g.TILE_PIXELS;
    player_hurtbox.y *= g.TILE_PIXELS;
    player_hurtbox.width *= g.TILE_PIXELS;
    player_hurtbox.height *= g.TILE_PIXELS;
    DrawRectanglePro(player_hurtbox, {}, {}, {255, 0, 0, 100});

    EndMode2D();
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

    g.level = load_level(LEVELS "1.csv");

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