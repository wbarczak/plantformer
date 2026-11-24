#pragma once

#include <cstdint>

static constexpr int32_t WINDOW_WIDTH = 320;
static constexpr int32_t WINDOW_HEIGHT = 180;
inline const char* WINDOW_NAME = "Plantformer";
static constexpr float DEFAULT_WINDOW_SCALE = 4;
static constexpr float PIXELS_PER_TILE = 16;
static constexpr int32_t TICKRATE = 60;
static constexpr float DT = 1.f / TICKRATE;