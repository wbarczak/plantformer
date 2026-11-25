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
    Camera2D cam{.offset{static_cast<float>(WINDOW_WIDTH) / 2, static_cast<float>(WINDOW_HEIGHT) / 2}, .zoom = 1};

    TileId selected{};
}game;

void respawn(Entity& entity){
    entity.respawning = true;
    if(get_tile(game.level, entity.spawnpoint.x, entity.spawnpoint.y) == TileId::Checkpoint){
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
    hurtbox.x += entity.vel.x * DT;

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
        TileId tile_id = get_tile(game.level, start_x, y);
        auto& tile = global.tiles.at(tile_id);

        bool break_out{};
        switch(tile.colision){
            case TileColision::Solid:{
                hurtbox.x = start_x + 1 + game.SKIN_WIDTH;
                entity.vel.x = 0;
                break_out = true;
                break;
            }
            case TileColision::Hazard:{
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
        TileId tile_id = get_tile(game.level, end_x, y);
        auto& tile = global.tiles.at(tile_id);

        bool break_out{};
        switch(tile.colision){
            case TileColision::Solid:{
                hurtbox.x = end_x - hurtbox.width - game.SKIN_WIDTH;
                entity.vel.x = 0;
                break_out = true;
                break;
            }
            case TileColision::Hazard:{
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
    hurtbox.y += entity.vel.y * DT;

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
        TileId tile_id = get_tile(game.level, x, start_y);
        auto& tile = global.tiles.at(tile_id);

        bool break_out{};
        switch(tile.colision){
            case TileColision::Solid:{
                hurtbox.y = start_y + 1 + game.SKIN_WIDTH;
                //entity.vel.y = 0;
                break_out = true;
                break;
            }
            case TileColision::Hazard:{
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
        TileId tile_id = get_tile(game.level, x, end_y);
        auto& tile = global.tiles.at(tile_id);

        bool break_out{};
        auto ground_colision = [&]{
            hurtbox.y = end_y - hurtbox.height - game.SKIN_WIDTH;
            entity.vel.y = 0;
            entity.grounded = true;
            break_out = true;
        };

        switch(tile.colision){
            case TileColision::Solid:{
                ground_colision();
                break;
            }
            case TileColision::Hazard:{
                respawn(entity);
                break;
            }
            case TileColision::Platform:{
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
    game.level.flat.fill(TileId::None);
    for(size_t y = 50; y < Level::HEIGHT; y++){
        for(size_t x{}; x < Level::WIDTH; x++){
            get_tile(game.level, x, y) = TileId::Dirt;
        }
    }
}

void game_on_exit(){

}

void game_update(){
    if(IsKeyPressed(KEY_C)){
        game.selected = static_cast<TileId>(static_cast<size_t>(game.selected + 1) % TILE_COUNT);
    }

    Vector2 cursor_tile_pos = (global.input.mouse - game.cam.offset + game.cam.target) / PIXELS_PER_TILE;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        get_tile(game.level, cursor_tile_pos.x, cursor_tile_pos.y) = TileId::None;
    }
    else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
        TileId& tile = get_tile(game.level, cursor_tile_pos.x, cursor_tile_pos.y);
        if(tile == TileId::None){
            tile = game.selected;
        }
    }

    auto& input = global.input;

    if(input.direction_x < 0){
        game.player.vel.x -= game.PLAYER_ACCELERATION * DT;
        game.player.vel.x = std::max(game.player.vel.x, -game.PLAYER_SPEED);
    }
    else if(input.direction_x > 0){
        game.player.vel.x += game.PLAYER_ACCELERATION * DT;
        game.player.vel.x = std::min(game.player.vel.x, game.PLAYER_SPEED);
    }
    else if(game.player.vel.x < 0){
        game.player.vel.x += game.PLAYER_DECELERATION * DT;
        game.player.vel.x = std::min(game.player.vel.x, 0.f);
    }
    else if (game.player.vel.x > 0){
        game.player.vel.x -= game.PLAYER_DECELERATION * DT;
        game.player.vel.x = std::max(game.player.vel.x, 0.f);
    }
    
    game.player.vel.y += game.GRAVITY_ACCELERATION * DT;
    if (global.input.jump_pressed && game.player.grounded){
        game.player.vel.y = -game.JUMP_ACCELERATION;
    }
    else if (game.player.vel.y > 0){
        game.player.vel.y += game.GRAVITY_ACCELERATION * DT;
        game.player.vel.y = std::min(game.player.vel.y, game.GRAVITY_SPEED);
    }
    else if(!input.jump_held){
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
    if (get_tile(game.level, player_position.x, player_position.y) == TileId::Checkpoint){
        game.player.spawnpoint = player_position;
    }
}

void game_draw(){
    DrawTexture(global.assets.background, 0, 0, WHITE);
    
    game.cam.target = game.player.pos * PIXELS_PER_TILE;
    game.cam.target.x = std::max(game.cam.target.x, 0.f + game.cam.offset.x);
    game.cam.target.y = std::max(game.cam.target.y, 0.f + game.cam.offset.y);
    game.cam.target.x = std::min(game.cam.target.x, Level::WIDTH * PIXELS_PER_TILE - game.cam.offset.x);
    game.cam.target.y = std::min(game.cam.target.y, Level::HEIGHT * PIXELS_PER_TILE - game.cam.offset.y);
    BeginMode2D(game.cam);

    for(int32_t y{}; y < Level::HEIGHT; y++){
        for(int32_t x{}; x < Level::WIDTH; x++){
            TileId tile_id = get_tile(game.level, x, y);
            TileSprite& sprite = global.assets.tile_sprites.at(tile_id);

            auto draw = [&](Rectangle src){
                DrawTexturePro(
                    sprite.tex,
                    src,
                    {x * PIXELS_PER_TILE, y * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE}, 
                    {},
                    {},
                    WHITE
                );
            };

            switch(sprite.type){
                case TileSpriteType::Normal:
                    draw(sprite.normal);
                    break;
                case TileSpriteType::SpecialTop:{
                    int32_t test_y = y - 1;
                    if(test_y < 0 || get_tile(game.level, x, test_y) != tile_id){
                        draw(sprite.top);
                        break;
                    }
                    draw(sprite.normal);
                    break;
                }
                case TileSpriteType::Attached:{
                    auto is_solid = [&](int32_t test_x, int32_t test_y){
                        if (test_x < 0 || test_y < 0 || test_x >= Level::WIDTH || test_y >= Level::HEIGHT){
                            return true;
                        }

                        auto& tile_colision = global.tiles.at(get_tile(game.level, test_x, test_y)).colision;
                        return tile_colision == TileColision::Solid || (tile_colision == TileColision::Platform && test_y == y + 1);
                    };

                    if(is_solid(x, y + 1)){
                        draw(sprite.up);
                    }
                    else if(is_solid(x, y - 1)){
                        draw(sprite.down);
                    }
                    else if(is_solid(x - 1, y)){
                        draw(sprite.right);
                    }
                    else if(is_solid(x + 1, y)){
                        draw(sprite.left);
                    }
                    else{
                        draw(sprite.normal);
                    }
                    break;
                }
                case TileSpriteType::Checkpoint:
                    if(game.player.spawnpoint.x == x && game.player.spawnpoint.y == y){
                        draw(sprite.active);
                    }
                    else{
                        draw(sprite.normal);
                    }
                    break;
            }
        }
    }

    Rectangle player_rect = global.assets.player.rect;
    if (game.player.facing_left){
        player_rect.width *= -1;
    }

    DrawTexturePro(
        global.assets.player.tex,
        player_rect,
        {(game.player.pos.x - .5f) * PIXELS_PER_TILE, (game.player.pos.y - 1) * PIXELS_PER_TILE, 16, 16},
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
    
    // Sprite selected{};
    // Rectangle selected_dest{0, 0, 16, 16};
    // switch (game.selected){
    //     case TileId::DIRT:
    //         selected = global.assets.sprites.grass;
    //         break;
    //     case TileId::PLATFORM:
    //         selected = global.assets.sprites.platform;
    //         break;
    //     case TileId::SPIKE:
    //         selected = global.assets.sprites.spike_up;
    //         break;
    //     case TileId::CHECKPOINT:
    //         selected = global.assets.sprites.checkpoint;
    //         break;
    //     default:
    //         break;
    // }

    // DrawTexturePro(selected.tex, selected.rect, selected_dest, {}, {}, WHITE);
    // DrawRectangleLinesEx(selected_dest, 1, WHITE);
}