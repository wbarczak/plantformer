#include "game.hpp"

#include <cmath>

#include <raylib.h>
#include <raymath.h>

#include "../Global.hpp"
#include "../Level.hpp"

struct Entity{
    Vector2 pos{3, 3};
    Vector2 vel{};
    Rectangle hurtbox{-.25, -.75, .5, .75};
    bool grounded = false;
    bool facing_left = false;
};

static struct GameData{
    static constexpr float PLAYER_SPEED = 6;
    static constexpr float PLAYER_ACCELERATION = 30;
    static constexpr float PLAYER_DECELERATION = 30;
    static constexpr float GRAVITY_SPEED = 15;
    static constexpr float GRAVITY_ACCELERATION = 20;
    static constexpr float JUMP_ACCELERATION = 11;
    static constexpr float SKIN_WIDTH = 1e-4f;

    Level level;
    Entity player;
    Camera2D cam{.offset{static_cast<float>(g.WIDTH) / 2, static_cast<float>(g.HEIGHT) / 2}, .zoom = 1};
}game;

void colisions_x(Entity& entity){
    Rectangle starting_hurtbox = entity.hurtbox;
    starting_hurtbox.x += entity.pos.x;
    starting_hurtbox.y += entity.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.x += entity.vel.x * g.DELTA_TIME;

    if(hurtbox.x < 0){
        hurtbox.x = 0 + game.SKIN_WIDTH;
        entity.vel.x = 0;
    }
    if (hurtbox.x + hurtbox.width > game.level.WIDTH){
        hurtbox.x = game.level.WIDTH - hurtbox.width - game.SKIN_WIDTH;
        entity.vel.x = 0;
    }
    
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;
    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;

    for (int32_t y = start_y; y <= end_y; y++){
        TileId tile = get_tile(game.level, start_x, y);
        if(!valid_tile(tile)){
            continue;
        }
        TileDef def = g.tile_defs[index(tile)];

        bool break_out{};
        switch(def.type){
            case TileType::SOLID:{
                hurtbox.x = start_x + 1 + game.SKIN_WIDTH;
                entity.vel.x = 0;
                break_out = true;
                break;
            }
            default:{
                break;
            }
        }

        if(break_out){
            break;
        }
    }

    start_x = hurtbox.x;
    end_x = hurtbox.x + hurtbox.width;

    for (int32_t y = start_y; y <= end_y; y++){
        TileId tile = get_tile(game.level, end_x, y);
        if(!valid_tile(tile)){
            continue;
        }
        TileDef def = g.tile_defs[index(tile)];

        bool break_out{};
        switch(def.type){
            case TileType::SOLID:{
                hurtbox.x = end_x - hurtbox.width - game.SKIN_WIDTH;
                entity.vel.x = 0;
                break_out = true;
                break;
            }
            default:{
                break;
            }
        }

        if(break_out){
            break;
        }
    }

    entity.pos.x += hurtbox.x - starting_hurtbox.x;
}

void colisions_y(Entity& entity){
    Rectangle starting_hurtbox = entity.hurtbox;
    starting_hurtbox.x += entity.pos.x;
    starting_hurtbox.y += entity.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.y += entity.vel.y * g.DELTA_TIME;

    entity.grounded = false;

    if(hurtbox.y < 0){
        hurtbox.y = 0 + game.SKIN_WIDTH;
        //entity.vel.y = 0;
    }
    if (hurtbox.y + hurtbox.height > game.level.HEIGHT){
        hurtbox.y = game.level.HEIGHT - hurtbox.height - game.SKIN_WIDTH;
        entity.vel.y = 0;
        entity.grounded = true;
    }

    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;

    for (int32_t x = start_x; x <= end_x; x++){
        TileId tile = get_tile(game.level, x, start_y);
        if(!valid_tile(tile)){
            continue;
        }
        TileDef def = g.tile_defs[index(tile)];

        bool break_out{};
        switch(def.type){
            case TileType::SOLID:{
                hurtbox.y = start_y + 1 + game.SKIN_WIDTH;
                //entity.vel.y = 0;
                break_out = true;
                break;
            }
            default:{
                break;
            }
        }

        if(break_out){
            break;
        }
    }

    start_y = hurtbox.y;
    end_y = hurtbox.y + hurtbox.height;

    for (int32_t x = start_x; x <= end_x; x++){
        TileId tile = get_tile(game.level, x, end_y);
        if(!valid_tile(tile)){
            continue;
        }
        TileDef def = g.tile_defs[index(tile)];

        bool break_out{};
        auto ground_colision = [&]{
            hurtbox.y = end_y - hurtbox.height - game.SKIN_WIDTH;
            entity.vel.y = 0;
            entity.grounded = true;
            break_out = true;
        };
        switch(def.type){
            case TileType::SOLID:{
                ground_colision();
                break;
            }
            case TileType::PLATFORM:{
                if(floor(hurtbox.y + hurtbox.height) > floor(starting_hurtbox.y + starting_hurtbox.height)){
                    ground_colision();
                }
                break;
            }
            default:{
                break;
            }
        }

        if(break_out){
            break;
        }
    }

    entity.pos.y += hurtbox.y - starting_hurtbox.y;
}

void game_on_enter(){
    game.level.flat.fill(TileId::NONE);
    for (size_t i{}; i < Level::WIDTH; i++){
        get_tile(game.level, i, Level::HEIGHT - 1) = TileId::DIRT;
    }
}

void game_on_exit(){

}

void game_update(){
    if(g.input.direction_x < 0){
        game.player.vel.x -= game.PLAYER_ACCELERATION * g.DELTA_TIME;
        game.player.vel.x = std::max(game.player.vel.x, -game.PLAYER_SPEED);
    }
    else if(g.input.direction_x > 0){
        game.player.vel.x += game.PLAYER_ACCELERATION * g.DELTA_TIME;
        game.player.vel.x = std::min(game.player.vel.x, game.PLAYER_SPEED);
    }
    else if(game.player.vel.x < 0){
        game.player.vel.x += game.PLAYER_DECELERATION * g.DELTA_TIME;
        game.player.vel.x = std::min(game.player.vel.x, 0.f);
    }
    else if (game.player.vel.x > 0){
        game.player.vel.x -= game.PLAYER_DECELERATION * g.DELTA_TIME;
        game.player.vel.x = std::max(game.player.vel.x, 0.f);
    }
    
    game.player.vel.y += game.GRAVITY_ACCELERATION * g.DELTA_TIME;
    if (g.input.jump_pressed && game.player.grounded){
        game.player.vel.y = -game.JUMP_ACCELERATION;
    }
    else if (game.player.vel.y > 0){
        game.player.vel.y += game.GRAVITY_ACCELERATION * g.DELTA_TIME;
        game.player.vel.y = std::min(game.player.vel.y, game.GRAVITY_SPEED);
    }
    else if(!g.input.jump_held){
        game.player.vel.y = 0;
    }

    if(game.player.vel.x != 0){
        game.player.facing_left = game.player.vel.x < 0;
    }

    colisions_x(game.player);
    colisions_y(game.player);
}

void game_draw(){
    game.cam.target = game.player.pos * g.TILE_PIXELS;
    BeginMode2D(game.cam);

    for(int32_t y{}; y < Level::HEIGHT; y++){
        for(int32_t x{}; x < Level::WIDTH; x++){
            TileId tile = get_tile(game.level, x, y);
            Sprite sprite{};
            switch (tile){
                case TileId::DIRT: sprite = g.assets.sprites.dirt; break;
                case TileId::SPIKE: sprite = g.assets.sprites.spike; break;
                case TileId::PLATFORM: sprite = g.assets.sprites.platform; break;
            }
            DrawTexturePro(sprite.tex, sprite.rect, {x * g.TILE_PIXELS, y * g.TILE_PIXELS, 16, 16}, {}, {}, WHITE);
        }
    }

    Rectangle player_rect = g.assets.sprites.player.rect;
    if (game.player.facing_left){
        player_rect.width *= -1;
    }

    DrawTexturePro(
        g.assets.sprites.player.tex,
        player_rect,
        {(game.player.pos.x - .5f) * g.TILE_PIXELS, (game.player.pos.y - 1) * g.TILE_PIXELS, 16, 16},
        {},
        {},
        WHITE
    );

    Rectangle player_hurtbox = game.player.hurtbox;
    player_hurtbox.x += game.player.pos.x;
    player_hurtbox.y += game.player.pos.y;
    player_hurtbox.x *= g.TILE_PIXELS;
    player_hurtbox.y *= g.TILE_PIXELS;
    player_hurtbox.width *= g.TILE_PIXELS;
    player_hurtbox.height *= g.TILE_PIXELS;
    DrawRectanglePro(player_hurtbox, {}, {}, {255, 0, 0, 100});

    EndMode2D();
}