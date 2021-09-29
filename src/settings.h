#pragma once

#include "common.h"

struct Settings {
    u32 instructions_per_frame;
    u32 delay_decrement_per_frame;
    u8 screen_scale;
    u32 frames_per_second_ = 60;
    static constexpr u32 CHIP8_HISTORY_AMOUNT = 200;
    static constexpr u32 MAX_PAST_OPS = 35;
    static constexpr u32 FUTURE_OPS = 15;
};

