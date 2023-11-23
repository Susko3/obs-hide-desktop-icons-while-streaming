#pragma once
#include "bindable/bindable.h"
#include <bitset>

namespace state {

namespace bit_index {
constexpr size_t streaming_active = 0;
constexpr size_t recording_active = 1;
constexpr size_t display_capture = 2;
}

inline bindable<bool> desktop_icons_visible{true};
inline bindable<std::bitset<3>> current_state{std::bitset<3>()};
inline bindable<std::bitset<3>> settings_state{std::bitset<3>()};
void setup();
}
