/*
OBS Hide Desktop Icons While Streaming
Copyright (C) Susko3 <susko3@protonmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#pragma once
#include "bindable/bindable.h"
#include <bitset>
#include <optional>

namespace state
{

namespace bit_index
{
	constexpr size_t streaming_active = 0;
	constexpr size_t recording_active = 1;
	constexpr size_t display_capture = 2;
}

inline bindable<bool> desktop_icons_visible{true};
inline bindable<std::optional<bool>> force_desktop_icons_visible{std::nullopt};
inline bindable<std::bitset<3>> current_state{std::bitset<3>()};
inline bindable<std::bitset<3>> settings_state{std::bitset<3>()};
void setup();
void update_bit(size_t index, bool value);
}
