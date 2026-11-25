#pragma once

#include <cstdint>
#include <cmath>

#include <raylib.h>

#include "../Global.hpp"
#include "../World.hpp"
#include "../Constants.hpp"
#include "commands.hpp"

void inline colisions_x(const Tilemap& map, Entity& entity){
    const auto& tiles = global.tiles;
    
    Rectangle starting_hurtbox = entity.hurtbox;
    starting_hurtbox.x += entity.pos.x;
    starting_hurtbox.y += entity.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.x += entity.vel.x * DT;

    if(hurtbox.x < 0){
        hurtbox.x = 0 + SKIN_WIDTH;
        entity.vel.x = 0;
    }
    if (hurtbox.x + hurtbox.width > WORLD_WIDTH){
        hurtbox.x = WORLD_WIDTH - hurtbox.width - SKIN_WIDTH;
        entity.vel.x = 0;
    }
    
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;
    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;

    for (int32_t y = start_y; y <= end_y; y++){
        TileId tile_id = map[y][start_x];
        auto& tile = tiles.at(tile_id);

        switch(tile.colision){
            case TileColision::Solid:{
                hurtbox.x = start_x + 1 + SKIN_WIDTH;
                entity.vel.x = 0;
                break;
            }
            case TileColision::Hazard:{
                respawn(map, entity);
                break;
            }
            case TileColision::Air:
            case TileColision::Platform:
                break;
        }
    }

    start_x = hurtbox.x;
    end_x = hurtbox.x + hurtbox.width;

    for (int32_t y = start_y; y <= end_y; y++){
        TileId tile_id = map[y][end_x];
        auto& tile = tiles.at(tile_id);

        switch(tile.colision){
            case TileColision::Solid:{
                hurtbox.x = end_x - hurtbox.width - SKIN_WIDTH;
                entity.vel.x = 0;
                break;
            }
            case TileColision::Hazard:{
                respawn(map, entity);
                break;
            }
            case TileColision::Air:
            case TileColision::Platform:
                break;
        }
    }

    if(!entity.respawning){
        entity.pos.x += hurtbox.x - starting_hurtbox.x;
    }
}

void inline colisions_y(const Tilemap& map, Entity& entity){
    const auto& tiles = global.tiles;
    
    Rectangle starting_hurtbox = entity.hurtbox;
    starting_hurtbox.x += entity.pos.x;
    starting_hurtbox.y += entity.pos.y;
    Rectangle hurtbox = starting_hurtbox;
    hurtbox.y += entity.vel.y * DT;

    entity.grounded = false;

    if(hurtbox.y < 0){
        hurtbox.y = 0 + SKIN_WIDTH;
        //entity.vel.y = 0;
    }
    if (hurtbox.y + hurtbox.height > WORLD_HEIGHT){
        hurtbox.y = WORLD_HEIGHT - hurtbox.height - SKIN_WIDTH;
        entity.vel.y = 0;
        entity.grounded = true;
    }

    int32_t start_x = hurtbox.x;
    int32_t end_x = hurtbox.x + hurtbox.width;
    int32_t start_y = hurtbox.y;
    int32_t end_y = hurtbox.y + hurtbox.height;

    for (int32_t x = start_x; x <= end_x; x++){
        TileId tile_id = map[start_y][x];
        auto& tile = tiles.at(tile_id);

        switch(tile.colision){
            case TileColision::Solid:{
                hurtbox.y = start_y + 1 + SKIN_WIDTH;
                //entity.vel.y = 0;
                break;
            }
            case TileColision::Hazard:{
                respawn(map, entity);
                break;
            }
            case TileColision::Air:
            case TileColision::Platform:
                break;
        }
    }

    start_y = hurtbox.y;
    end_y = hurtbox.y + hurtbox.height;

    for (int32_t x = start_x; x <= end_x; x++){
        TileId tile_id = map[end_y][x];
        auto& tile = tiles.at(tile_id);

        auto ground_colision = [&]{
            hurtbox.y = end_y - hurtbox.height - SKIN_WIDTH;
            entity.vel.y = 0;
            entity.grounded = true;
        };

        switch(tile.colision){
            case TileColision::Solid:{
                ground_colision();
                break;
            }
            case TileColision::Hazard:{
                respawn(map, entity);
                break;
            }
            case TileColision::Platform:{
                if(global.input.dir.y >= Y_THRESHHOLD_TO_GO_DOWN_A_PLATFORM && global.input.jump_held){
                    break;
                }
                if(floor(hurtbox.y + hurtbox.height) > floor(starting_hurtbox.y + starting_hurtbox.height)){
                    ground_colision();
                }
                break;
            }
            case TileColision::Air:
                break;
        }
    }

    if(!entity.respawning){
        entity.pos.y += hurtbox.y - starting_hurtbox.y;
    }
}