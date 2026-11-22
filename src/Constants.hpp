#pragma once

#include <cstdint>

namespace k{
    static constexpr int32_t window_width = 320;
    static constexpr int32_t window_height = 180;
    inline const char* window_name = "Plantformer";
    static constexpr float default_window_scale = 4;
    static constexpr float pixels_per_tile = 16;
    static constexpr int32_t tickrate = 60;
    static constexpr float delta_time = 1.f / tickrate;
}