#pragma once

#include <cstdint>

static constexpr int32_t WINDOW_WIDTH = 320;
static constexpr int32_t WINDOW_HEIGHT = 180;
inline const char* WINDOW_NAME = "Plantformer";
static constexpr float DEFAULT_WINDOW_SCALE = 4;

static constexpr float PIXELS_PER_TILE = 16;

static constexpr int32_t TICKRATE = 60;
static constexpr float DT = 1.f / TICKRATE;

static constexpr int32_t WORLD_WIDTH = 100;
static constexpr int32_t WORLD_HEIGHT = 100;

static constexpr float PLAYER_SPEED = 6;
static constexpr float PLAYER_ACCELERATION = 30;
static constexpr float PLAYER_DECELERATION = 30;
static constexpr float GRAVITY_SPEED = 15;
static constexpr float GRAVITY_ACCELERATION = 20;
static constexpr float JUMP_ACCELERATION = 11;
static constexpr float SKIN_WIDTH = 1e-4f;

static constexpr float Y_THRESHHOLD_TO_GO_DOWN_A_PLATFORM = .8;