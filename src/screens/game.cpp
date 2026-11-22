#include "game.hpp"

#include <cmath>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "../Global.hpp"
#include "../Level.hpp"
#include "../Constants.hpp"

struct Position{
    int32_t x{}, y{};
};

struct Entity{
    Vector2 pos{50, 40};
    Vector2 vel{};
    Rectangle hurtbox{-.25, -.75, .5, .75};
    Position spawnpoint;
    bool grounded = false;
    bool facing_left = false;
    bool respawning = false;
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
    Camera2D cam{.offset{static_cast<float>(k::window_width) / 2, static_cast<float>(k::window_height) / 2}, .zoom = 1};

    TileId selected{};
}game;

void respawn(Entity& entity){
    entity.respawning = true;
    if(get_tile(game.level, entity.spawnpoint.x, entity.spawnpoint.y) == TileId::CHECKPOINT){
        entity.pos.x = entity.spawnpoint.x + .5;
        entity.pos.y = entity.spawnpoint.y + 1 - game.SKIN_WIDTH;
    }
    else{
        entity.pos = {};
    }
    entity.vel = {};
}

void colisions_x(Entity& entity){
    Rectangle starting_hurtbox = entity.hurtbox;
    starting_hurtbox.x += entity.pos.x;
    starting_hurtbox.y += entity.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.x += entity.vel.x * k::delta_time;

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
        switch(def.colision){
            case TileColision::SOLID:{
                hurtbox.x = start_x + 1 + game.SKIN_WIDTH;
                entity.vel.x = 0;
                break_out = true;
                break;
            }
            case TileColision::HAZARD:{
                respawn(entity);
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
        switch(def.colision){
            case TileColision::SOLID:{
                hurtbox.x = end_x - hurtbox.width - game.SKIN_WIDTH;
                entity.vel.x = 0;
                break_out = true;
                break;
            }
            case TileColision::HAZARD:{
                respawn(entity);
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

    if(!entity.respawning){
        entity.pos.x += hurtbox.x - starting_hurtbox.x;
    }
}

void colisions_y(Entity& entity){
    Rectangle starting_hurtbox = entity.hurtbox;
    starting_hurtbox.x += entity.pos.x;
    starting_hurtbox.y += entity.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.y += entity.vel.y * k::delta_time;

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
        switch(def.colision){
            case TileColision::SOLID:{
                hurtbox.y = start_y + 1 + game.SKIN_WIDTH;
                //entity.vel.y = 0;
                break_out = true;
                break;
            }
            case TileColision::HAZARD:{
                respawn(entity);
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

        switch(def.colision){
            case TileColision::SOLID:{
                ground_colision();
                break;
            }
            case TileColision::HAZARD:{
                respawn(entity);
                break;
            }
            case TileColision::PLATFORM:{
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

    if(!entity.respawning){
        entity.pos.y += hurtbox.y - starting_hurtbox.y;
    }
}

void game_on_enter(){
    game.level.flat.fill(TileId::NONE);
    for(size_t y = 50; y < Level::HEIGHT; y++){
        for(size_t x{}; x < Level::WIDTH; x++){
            get_tile(game.level, x, y) = TileId::DIRT;
        }
    }
}

void game_on_exit(){

}

void game_update(){
    if(IsKeyPressed(KEY_C)){
        game.selected = static_cast<TileId>((index(game.selected) + 1) % index(TileId::COUNT));
    }

    Vector2 cursor_tile_pos = (g.input.mouse - game.cam.offset + game.cam.target) / k::pixels_per_tile;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        get_tile(game.level, cursor_tile_pos.x, cursor_tile_pos.y) = TileId::NONE;
    }
    else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
        TileId& tile = get_tile(game.level, cursor_tile_pos.x, cursor_tile_pos.y);
        if(tile == TileId::NONE){
            tile = game.selected;
        }
    }

    if(g.input.direction_x < 0){
        game.player.vel.x -= game.PLAYER_ACCELERATION * k::delta_time;
        game.player.vel.x = std::max(game.player.vel.x, -game.PLAYER_SPEED);
    }
    else if(g.input.direction_x > 0){
        game.player.vel.x += game.PLAYER_ACCELERATION * k::delta_time;
        game.player.vel.x = std::min(game.player.vel.x, game.PLAYER_SPEED);
    }
    else if(game.player.vel.x < 0){
        game.player.vel.x += game.PLAYER_DECELERATION * k::delta_time;
        game.player.vel.x = std::min(game.player.vel.x, 0.f);
    }
    else if (game.player.vel.x > 0){
        game.player.vel.x -= game.PLAYER_DECELERATION * k::delta_time;
        game.player.vel.x = std::max(game.player.vel.x, 0.f);
    }
    
    game.player.vel.y += game.GRAVITY_ACCELERATION * k::delta_time;
    if (g.input.jump_pressed && game.player.grounded){
        game.player.vel.y = -game.JUMP_ACCELERATION;
    }
    else if (game.player.vel.y > 0){
        game.player.vel.y += game.GRAVITY_ACCELERATION * k::delta_time;
        game.player.vel.y = std::min(game.player.vel.y, game.GRAVITY_SPEED);
    }
    else if(!g.input.jump_held){
        game.player.vel.y = 0;
    }

    if(game.player.vel.x != 0){
        game.player.facing_left = game.player.vel.x < 0;
    }

    game.player.respawning= false;
    colisions_x(game.player);
    colisions_y(game.player);

    Position player_position;
    player_position.x = game.player.pos.x;
    player_position.y = game.player.pos.y - game.SKIN_WIDTH;
    if (get_tile(game.level, player_position.x, player_position.y) == TileId::CHECKPOINT){
        game.player.spawnpoint = player_position;
    }
}

void game_draw(){
    DrawTexture(g.assets.background, 0, 0, WHITE);
    
    game.cam.target = game.player.pos * k::pixels_per_tile;
    game.cam.target.x = std::max(game.cam.target.x, 0.f + game.cam.offset.x);
    game.cam.target.y = std::max(game.cam.target.y, 0.f + game.cam.offset.y);
    game.cam.target.x = std::min(game.cam.target.x, Level::WIDTH * k::pixels_per_tile - game.cam.offset.x);
    game.cam.target.y = std::min(game.cam.target.y, Level::HEIGHT * k::pixels_per_tile - game.cam.offset.y);
    BeginMode2D(game.cam);

    for(int32_t y{}; y < Level::HEIGHT; y++){
        for(int32_t x{}; x < Level::WIDTH; x++){
            TileId tile = get_tile(game.level, x, y);
            Sprite sprite{};
            switch (tile){
                case TileId::DIRT:{
                    TileId tile = get_tile(game.level, x, y - 1);
                    bool valid = valid_tile(tile);
                    if (y > 0 && ((valid && g.tile_defs[index(tile)].colision != TileColision::SOLID) || !valid)){
                        sprite = g.assets.sprites.grass;
                    }
                    else{
                        sprite = g.assets.sprites.dirt;
                    }
                    break;
                }
                case TileId::SPIKE:{
                    TileId tile = get_tile(game.level, x, y + 1);
                    if (y < Level::HEIGHT && valid_tile(tile) && (g.tile_defs[index(tile)].colision == TileColision::SOLID || g.tile_defs[index(tile)].colision == TileColision::PLATFORM)){
                        sprite = g.assets.sprites.spike_up;
                        break;
                    }
                    tile = get_tile(game.level, x, y - 1);
                    if (y > 0 && valid_tile(tile) && g.tile_defs[index(tile)].colision == TileColision::SOLID){
                        sprite = g.assets.sprites.spike_down;
                        break;
                    }
                    tile = get_tile(game.level, x + 1, y);
                    if (x < Level::WIDTH && valid_tile(tile) && g.tile_defs[index(tile)].colision == TileColision::SOLID){
                        sprite = g.assets.sprites.spike_left;
                        break;
                    }
                    tile = get_tile(game.level, x - 1, y);
                    if (x > 0 && valid_tile(tile) && g.tile_defs[index(tile)].colision == TileColision::SOLID){
                        sprite = g.assets.sprites.spike_right;
                        break;
                    }
                    sprite = g.assets.sprites.spike;
                    break;
                }
                case TileId::PLATFORM: sprite = g.assets.sprites.platform; break;
                case TileId::CHECKPOINT:{
                    if(game.player.spawnpoint.x == x && game.player.spawnpoint.y == y){
                        sprite = g.assets.sprites.checkpoint_active;
                        break;
                    }
                    sprite = g.assets.sprites.checkpoint;
                }
            }
            DrawTexturePro(sprite.tex, sprite.rect, {x * k::pixels_per_tile, y * k::pixels_per_tile, 16, 16}, {}, {}, WHITE);
        }
    }

    Rectangle player_rect = g.assets.sprites.player.rect;
    if (game.player.facing_left){
        player_rect.width *= -1;
    }

    DrawTexturePro(
        g.assets.sprites.player.tex,
        player_rect,
        {(game.player.pos.x - .5f) * k::pixels_per_tile, (game.player.pos.y - 1) * k::pixels_per_tile, 16, 16},
        {},
        {},
        WHITE
    );

    // Rectangle player_hurtbox = game.player.hurtbox;
    // player_hurtbox.x += game.player.pos.x;
    // player_hurtbox.y += game.player.pos.y;
    // player_hurtbox.x *= g.TILE_PIXELS;
    // player_hurtbox.y *= g.TILE_PIXELS;
    // player_hurtbox.width *= g.TILE_PIXELS;
    // player_hurtbox.height *= g.TILE_PIXELS;
    // DrawRectanglePro(player_hurtbox, {}, {}, {255, 0, 0, 100});

    EndMode2D();
    
    Sprite selected{};
    Rectangle selected_dest{0, 0, 16, 16};
    switch (game.selected){
        case TileId::DIRT:
            selected = g.assets.sprites.grass;
            break;
        case TileId::PLATFORM:
            selected = g.assets.sprites.platform;
            break;
        case TileId::SPIKE:
            selected = g.assets.sprites.spike_up;
            break;
        case TileId::CHECKPOINT:
            selected = g.assets.sprites.checkpoint;
            break;
        default:
            break;
    }

    DrawTexturePro(selected.tex, selected.rect, selected_dest, {}, {}, WHITE);
    DrawRectangleLinesEx(selected_dest, 1, WHITE);
}