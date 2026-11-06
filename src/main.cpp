#include <cstdint>
#include <vector>

#include <raylib.h>

struct Sprite{
    Texture tex{};
    Rectangle rect{};
};

struct Anim{
    Texture tex{};
    std::vector<Rectangle> frames{};
    float time{};
};

struct AnimPlayer{
    Anim* anim{};
    float time{};
};

struct{
    Texture knight;
}g_assets;

enum BlockType{
    BLOCK_AIR,
    BLOCK_GRASS
};

struct global{
    static constexpr int32_t WIDTH = 320;
    static constexpr int32_t HEIGHT = 180;
    const char* WINDOW_NAME = "Platformer";

    struct{
        Texture knight;
    }assets;

    BlockType map[10][10];
}g;

void load_assets(){
    g.assets.knight = LoadTexture(ASSETS "sprites/knight.png");
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
    InitWindow(g.WIDTH, g.HEIGHT, g.WINDOW_NAME);

    RenderTexture2D target = LoadRenderTexture(g.WIDTH, g.HEIGHT);
    load_assets();

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        BeginTextureMode(target);
        ClearBackground(BLACK);

        EndTextureMode();

        flip_and_draw_target(target);

        EndDrawing();
    }

    CloseWindow();
}