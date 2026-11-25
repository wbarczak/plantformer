#include "game.hpp"

#include <cmath>

#include <raylib.h>
#include <raymath.h>

#include "../Global.hpp"
#include "../World.hpp"
#include "../Constants.hpp"
#include "../gameplay/colisions.hpp"

static struct GameData{
    World world;
    Camera2D cam{.offset{static_cast<float>(WINDOW_WIDTH) / 2, static_cast<float>(WINDOW_HEIGHT) / 2}, .zoom = 1};

    TileId selected{};
}game;

void game_on_enter(){
    game.world = make_new_world();
}

void game_on_exit(){

}

void game_update(){
    const auto& input = global.input;
    auto& world = game.world;

    if(input.cycle_pressed){
        game.selected = static_cast<TileId>(static_cast<size_t>(game.selected + 1) % TILE_COUNT);
    }

    Vector2 cursor_tile_pos = (input.mouse - game.cam.offset + game.cam.target) / PIXELS_PER_TILE;
    if(input.break_held){
        world.map[cursor_tile_pos.y][cursor_tile_pos.x] = TileId::None;
    }
    else if(input.place_held){
        TileId& tile = world.map[cursor_tile_pos.y][cursor_tile_pos.x];
        if(tile == TileId::None){
            tile = game.selected;
        }
    }

    if(input.dir.x < 0){
        world.player.vel.x -= PLAYER_ACCELERATION * DT;
        world.player.vel.x = std::max(world.player.vel.x, -PLAYER_SPEED);
    }
    else if(input.dir.x > 0){
        world.player.vel.x += PLAYER_ACCELERATION * DT;
        world.player.vel.x = std::min(world.player.vel.x, PLAYER_SPEED);
    }
    else if(world.player.vel.x < 0){
        world.player.vel.x += PLAYER_DECELERATION * DT;
        world.player.vel.x = std::min(world.player.vel.x, 0.f);
    }
    else if (world.player.vel.x > 0){
        world.player.vel.x -= PLAYER_DECELERATION * DT;
        world.player.vel.x = std::max(world.player.vel.x, 0.f);
    }
    
    world.player.vel.y += GRAVITY_ACCELERATION * DT;
    if (input.jump_pressed && world.player.grounded){
        world.player.vel.y = -JUMP_ACCELERATION;
    }
    else if (world.player.vel.y > 0){
        world.player.vel.y += GRAVITY_ACCELERATION * DT;
        world.player.vel.y = std::min(world.player.vel.y, GRAVITY_SPEED);
    }
    else if(!input.jump_held){
        world.player.vel.y = 0;
    }

    if(world.player.vel.x != 0){
        world.player.facing_left = world.player.vel.x < 0;
    }

    world.player.respawning= false;
    colisions_x(world.map, world.player);
    colisions_y(world.map, world.player);

    Position player_position;
    player_position.x = world.player.pos.x;
    player_position.y = world.player.pos.y - SKIN_WIDTH;
    if (world.map[player_position.y][player_position.x] == TileId::Checkpoint){
        world.player.spawnpoint = player_position;
    }
}

void game_draw(){
    const auto& world = game.world;

    DrawTexture(global.assets.background, 0, 0, WHITE);
    
    game.cam.target = world.player.pos * PIXELS_PER_TILE;
    game.cam.target.x = std::max(game.cam.target.x, 0.f + game.cam.offset.x);
    game.cam.target.y = std::max(game.cam.target.y, 0.f + game.cam.offset.y);
    game.cam.target.x = std::min(game.cam.target.x, WORLD_WIDTH * PIXELS_PER_TILE - game.cam.offset.x);
    game.cam.target.y = std::min(game.cam.target.y, WORLD_HEIGHT * PIXELS_PER_TILE - game.cam.offset.y);
    BeginMode2D(game.cam);

    for(int32_t y{}; y < WORLD_HEIGHT; y++){
        for(int32_t x{}; x < WORLD_WIDTH; x++){
            TileId tile_id = world.map[y][x];
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
                    if(test_y < 0 || world.map[test_y][x] != tile_id){
                        draw(sprite.top);
                        break;
                    }
                    draw(sprite.normal);
                    break;
                }
                case TileSpriteType::Attached:{
                    auto is_solid = [&](int32_t test_x, int32_t test_y){
                        if (test_x < 0 || test_y < 0 || test_x >= WORLD_WIDTH || test_y >= WORLD_HEIGHT){
                            return true;
                        }

                        auto& tile_colision = global.tiles.at(world.map[test_y][test_x]).colision;
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
                    if(world.player.spawnpoint.x == x && world.player.spawnpoint.y == y){
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
    if (world.player.facing_left){
        player_rect.width *= -1;
    }

    DrawTexturePro(
        global.assets.player.tex,
        player_rect,
        {(world.player.pos.x - .5f) * PIXELS_PER_TILE, (world.player.pos.y - 1) * PIXELS_PER_TILE, 16, 16},
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