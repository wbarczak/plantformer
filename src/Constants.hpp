#pragma once

#include <cstdint>
#include <cstdlib>

static constexpr int32_t WINDOW_WIDTH = 640;
static constexpr int32_t WINDOW_HEIGHT = 360;
inline const char* WINDOW_NAME = "Plantformer";
static constexpr float DEFAULT_WINDOW_SCALE = 2;

static constexpr float PIXELS_PER_TILE = 32;
static constexpr float PIXELS_PER_MINI_ICON = PIXELS_PER_TILE / 2;

static constexpr int32_t TICKRATE = 60;
static constexpr float DT = 1.f / TICKRATE;

static constexpr int32_t WORLD_WIDTH = 100;
static constexpr int32_t WORLD_HEIGHT = 60;

static constexpr float PLAYER_SPEED = 6;
static constexpr float PLAYER_ACCELERATION = 30;
static constexpr float PLAYER_DECELERATION = 30;
static constexpr float GRAVITY_SPEED = 15;
static constexpr float GRAVITY_ACCELERATION = 20;
static constexpr float JUMP_ACCELERATION = 11;
static constexpr float SKIN_WIDTH = 1e-4f;

static constexpr float Y_THRESHHOLD_TO_GO_DOWN_A_PLATFORM = .8;

static constexpr int32_t INVENTORY_ITEM_COUNT_ON_X_AXIS = 5;
static constexpr int32_t INVENTORY_ITEM_COUNT_ON_Y_AXIS = 3;
static constexpr int32_t MAX_STACK_SIZE = 1000;
static constexpr size_t TOOLBAR_INVENTORY_ROW = 0;