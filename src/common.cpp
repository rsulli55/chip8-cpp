#include "common.h"

auto random_byte() -> u8 {
    static std::random_device rd;
    static std::mt19937 rng {rd()};
    static std::uniform_int_distribution<int> dist {0, 255};

    return dist(rng);
}
