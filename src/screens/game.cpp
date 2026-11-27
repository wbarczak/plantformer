#include "game.hpp"

#include <cmath>
#include <algorithm>
#include <string>

#include <raylib.h>
#include <raymath.h>

#include "../Global.hpp"
#include "../World.hpp"
#include "../Constants.hpp"
#include "../gameplay/colisions.hpp"

static struct GameData{
    World world;
    Camera2D cam{.offset{static_cast<float>(WINDOW_WIDTH) / 2, static_cast<float>(WINDOW_HEIGHT) / 2}, .zoom = 1};

    int32_t selected{};

    bool inv_on{};
    ItemStack mouse{};
}game;

static constexpr int32_t OFFSET_BETWEEN_SLOTS = 2;

void game_on_enter(){
    game.world = make_new_world();
    game.world.inv = make_new_inventory();
}

void game_on_exit(){

}

void game_update(){
    const auto& input = global.input;
    auto& world = game.world;

    if(input.toggle_inventory){
        game.inv_on = !game.inv_on;
        if(!game.inv_on){
            bool break_out{};
            for(auto& row : world.inv.items){
                for(auto& item : row){
                    if(item.item_id == ItemId::Air){
                        item = game.mouse;
                        game.mouse = {};
                        break_out = true;
                        break;
                    }
                }
                if(break_out){
                    break;
                }
            }
        }
    }

    bool clicked_inv{};
    const int32_t inv_rows_to_display = game.inv_on? INVENTORY_ITEM_COUNT_ON_Y_AXIS : 1;
    Rectangle slot_rect{.width = PIXELS_PER_TILE, .height = PIXELS_PER_TILE};
    const int32_t which_x = static_cast<int32_t>(input.mouse.x) / (static_cast<int32_t>(PIXELS_PER_TILE) + OFFSET_BETWEEN_SLOTS);
    const int32_t which_y = static_cast<int32_t>(input.mouse.y) / (static_cast<int32_t>(PIXELS_PER_TILE) + OFFSET_BETWEEN_SLOTS);
    if(which_x >= 0 && which_x < INVENTORY_ITEM_COUNT_ON_X_AXIS && which_x >= 0 && which_y < inv_rows_to_display){
        slot_rect.x = which_x * (PIXELS_PER_TILE + OFFSET_BETWEEN_SLOTS);
        slot_rect.y = which_y * (PIXELS_PER_TILE + OFFSET_BETWEEN_SLOTS);

        if(CheckCollisionPointRec(input.mouse, slot_rect)){
            if(input.left_click){
                if(!game.inv_on){
                    game.selected = which_x;
                }
                else{
                    std::swap(game.mouse, world.inv.items[which_y][which_x]);
                }
            }
            clicked_inv = true;
        }
    }

    Vector2 cursor_tile_pos = (input.mouse - game.cam.offset + game.cam.target) / PIXELS_PER_TILE;
    if(input.break_held && !clicked_inv){
        TileId& tile = world.map[cursor_tile_pos.y][cursor_tile_pos.x];
        if(tile != TileId::None){
            ItemId dropped = global.loottables[tile];
            tile = TileId::None;
            Vector2 cursor_tile_middle = cursor_tile_pos;
            cursor_tile_middle.x = floorf(cursor_tile_middle.x) + .5f;
            cursor_tile_middle.y = floorf(cursor_tile_middle.y) + .5f;
            world.items.push_back(DroppedItem{.item_id = dropped, .position = cursor_tile_middle});
        }
    }
    else if(input.place_held && !clicked_inv){
        TileId& tile = world.map[cursor_tile_pos.y][cursor_tile_pos.x];
        ItemStack& selected_item = world.inv.items[TOOLBAR_INVENTORY_ROW][game.selected];

        if(tile == TileId::None){
            auto& item = global.items.at(selected_item.item_id);
            if(item.type == ItemType::Tile){
                tile = item.tile;
                inventory_decrease_stack(selected_item);
            }
        }
    }

    bool could_be_platforms_under_player = world.player.grounded && world.player.pos.y + SKIN_WIDTH < WORLD_HEIGHT;
    bool platforms_under_player{};
    if (could_be_platforms_under_player){
        const float y_position = world.player.pos.y + world.player.hurtbox.y + world.player.hurtbox.height + SKIN_WIDTH;
        auto& tile1 = global.tiles.at(world.map[y_position][world.player.pos.x + world.player.hurtbox.x]);
        auto& tile2 = global.tiles.at(world.map[y_position][world.player.pos.x + world.player.hurtbox.x + world.player.hurtbox.width]);
        
        platforms_under_player = (tile1.colision == TileColision::Air || tile1.colision == TileColision::Platform)
            && (tile2.colision == TileColision::Air || tile2.colision == TileColision::Platform);
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
    if (input.jump_pressed && world.player.grounded && !(platforms_under_player && input.dir.y >= Y_THRESHHOLD_TO_GO_DOWN_A_PLATFORM)){
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

    Rectangle player_hurtbox = world.player.hurtbox;
    player_hurtbox.x += world.player.pos.x;
    player_hurtbox.y += world.player.pos.y;
    for(auto item = world.items.begin(); item != world.items.end();){
        if(CheckCollisionPointRec(item->position, player_hurtbox)){
            bool collected = inventory_pickup(world.inv, item->item_id);

            if(collected){
                item = world.items.erase(item);
                continue;
            }
        }
        item += 1;
    }
}

void game_draw(){
    const auto& world = game.world;

    DrawRectangleGradientEx({0,0,WINDOW_WIDTH,WINDOW_HEIGHT}, SKYBLUE, BLUE, BLUE, SKYBLUE);
    
    game.cam.target = world.player.pos * PIXELS_PER_TILE;
    game.cam.target.y -= PIXELS_PER_TILE * .5f;
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
        {(world.player.pos.x - .5f) * PIXELS_PER_TILE, (world.player.pos.y - 1) * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE},
        {},
        {},
        WHITE
    );

    for(auto& dropped : world.items){
        Sprite sprite = global.assets.mini_icon_sprites.at(dropped.item_id);
        Rectangle dropped_rect{
            .x = dropped.position.x * PIXELS_PER_TILE,
            .y = dropped.position.y * PIXELS_PER_TILE,
            .width = PIXELS_PER_MINI_ICON,
            .height = PIXELS_PER_MINI_ICON,
        };
        dropped_rect.y += sin(global.elapsed * 4) * 2;
        DrawTexturePro(sprite.tex, sprite.rect, dropped_rect, Vector2{PIXELS_PER_MINI_ICON, PIXELS_PER_MINI_ICON} / 2.f, {}, WHITE);
    }

    // Rectangle player_hurtbox = game.player.hurtbox;
    // player_hurtbox.x += game.player.pos.x;
    // player_hurtbox.y += game.player.pos.y;
    // player_hurtbox.x *= g.TILE_PIXELS;
    // player_hurtbox.y *= g.TILE_PIXELS;
    // player_hurtbox.width *= g.TILE_PIXELS;
    // player_hurtbox.height *= g.TILE_PIXELS;
    // DrawRectanglePro(player_hurtbox, {}, {}, {255, 0, 0, 100});

    EndMode2D();
    
    Rectangle slot_dest{.width = PIXELS_PER_TILE, .height = PIXELS_PER_TILE};
    for(auto& row : world.inv.items){
        for(auto& item : row){
            DrawRectangleRec(slot_dest, {0, 0, 0, 100});
            Sprite item_sprite = global.assets.item_sprites.at(item.item_id);
            DrawTexturePro(item_sprite.tex, item_sprite.rect, slot_dest, {}, {}, WHITE);
            if(&item == &world.inv.items[0][game.selected]){
                DrawRectangleLinesEx(slot_dest, 1, WHITE);
            }
            DrawText(std::to_string(item.count).c_str(), slot_dest.x, slot_dest.y, 8, BLACK);
            slot_dest.x += PIXELS_PER_TILE + OFFSET_BETWEEN_SLOTS;
        }
        if (!game.inv_on){
            break;
        }
        slot_dest.x = 0;
        slot_dest.y += PIXELS_PER_TILE + OFFSET_BETWEEN_SLOTS;
    }

    Sprite mouse_sprite = global.assets.item_sprites.at(game.mouse.item_id);
    Rectangle mouse_rect{global.input.mouse.x, global.input.mouse.y, PIXELS_PER_TILE, PIXELS_PER_TILE};
    DrawTexturePro(mouse_sprite.tex, mouse_sprite.rect, mouse_rect, {}, {}, WHITE);
    if(game.mouse.item_id != ItemId::Air){
        DrawRectangleLinesEx(mouse_rect, 1, WHITE);
    }
}