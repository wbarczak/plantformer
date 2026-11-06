#include <cstdint>
#include <algorithm>

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
};

struct Global{
    static constexpr int32_t WIDTH = 320;
    static constexpr int32_t HEIGHT = 180;
    const char* WINDOW_NAME = "Platformer";
    static constexpr float SCALE = 4;
    static constexpr float TILE_PIXELS = 16;
    static constexpr int32_t TICKRATE = 60;
    static constexpr float TICK_DURATION = 1.f / TICKRATE;
    static constexpr float PLAYER_SPEED = .1f;

    struct{
        Texture character;
    }assets;

    struct{
        bool left{};
        bool right{};
        bool up{};
        bool down{};
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

void load_assets(){
    g.assets.character = LoadTexture(ASSETS "sprites/knight.png");
}

void read_input(){
    g.input.left = IsKeyDown(KEY_A);
    g.input.right = IsKeyDown(KEY_D);
    g.input.up = IsKeyDown(KEY_W);
    g.input.down = IsKeyDown(KEY_S);
}

void tick(){
    if (g.input.up){
        g.player.pos.y -= g.PLAYER_SPEED;
    }
    if (g.input.down){
        g.player.pos.y += g.PLAYER_SPEED;
    }
    if (g.input.right){
        g.player.pos.x += g.PLAYER_SPEED;
    }
    if (g.input.left){
        g.player.pos.x -= g.PLAYER_SPEED;
    }
}

void draw(){
    for(int32_t y{}; y < 10; y++){
        for(int32_t x{}; x < 10; x++){
            if (g.map[y][x]){
                DrawRectangleGradientEx({x * g.TILE_PIXELS, y * g.TILE_PIXELS, g.TILE_PIXELS, g.TILE_PIXELS}, BLUE, DARKBLUE, GREEN, DARKGREEN);
            }
        }
    }

    DrawRectanglePro({g.player.pos.x * g.TILE_PIXELS - g.TILE_PIXELS / 2, g.player.pos.y * g.TILE_PIXELS - g.TILE_PIXELS, g.TILE_PIXELS, g.TILE_PIXELS}, {}, {}, RED);
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

    RenderTexture2D target = LoadRenderTexture(g.WIDTH, g.HEIGHT);
    load_assets();

    float timer{};

    while (!WindowShouldClose()){
        read_input();

        timer += GetFrameTime();
        timer = std::min(timer, .2f);
        while (timer >= g.TICK_DURATION){
            timer -= g.TICK_DURATION;
            tick();
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