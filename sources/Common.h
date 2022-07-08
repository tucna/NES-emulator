#pragma once

#include <array>
#include <cstdint>

using RAM2KB = std::array<uint8_t, 2 * 1024>; // 2 KB

constexpr uint8_t CARRY_MASK    = 0b00000001;
constexpr uint8_t NEGATIVE_MASK = 0b10000000;

constexpr uint16_t LO_BYTE = 0b0000000011111111;
constexpr uint16_t HI_BYTE = 0b1111111100000000;
