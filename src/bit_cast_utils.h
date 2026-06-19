// Copyright 2025 Skate 3 ReXGlue Recompiled Project
// SPDX-License-Identifier: MIT
//
// Utility functions for bit-casting between floats and integers.
// Centralized to avoid duplication across FOV, ultrawide, and other systems.

#pragma once

#include <cstdint>
#include <bit>

namespace skate3 {

/// Convert a 32-bit integer representation to a float value.
/// This is equivalent to reinterpret_cast but safe and portable.
inline float FloatFromBits(uint32_t bits) {
    return std::bit_cast<float>(bits);
}

/// Convert a float value to its 32-bit integer representation.
/// This is equivalent to reinterpret_cast but safe and portable.
inline uint32_t BitsFromFloat(float value) {
    return std::bit_cast<uint32_t>(value);
}

}  // namespace skate3
