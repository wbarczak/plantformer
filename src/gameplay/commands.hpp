#pragma once

#include "../World.hpp"
#include "../Constants.hpp"

void inline respawn(const Tilemap& map, Entity& entity){
    entity.respawning = true;
    if(map[entity.spawnpoint.y][entity.spawnpoint.x] == TileId::Checkpoint){
        entity.pos.x = entity.spawnpoint.x + .5;
        entity.pos.y = entity.spawnpoint.y + 1 - SKIN_WIDTH;
    }
    else{
        entity.pos = {};
    }
    entity.vel = {};
}