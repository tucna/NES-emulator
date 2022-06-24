#pragma once

#include <array>
#include <cstdint>

using RAM2KB = std::array<uint8_t, 2 * 1024>; // 2 KB

constexpr uint8_t NEGATIVE_MASK = 0b10000000;