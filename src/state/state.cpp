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

#include "state.h"

namespace state
{

bool should_be_hidden()
{
	const auto settings = settings_state.get_value();
	const auto current = current_state.get_value();

	if (settings.none())
		return false;

	if (settings.test(bit_index::display_capture) && !current.test(bit_index::display_capture))
		return false;

	if ((settings & std::bitset<3>(0b011)).none())
		return true;

	const auto s_and_c = settings & current;

	if (s_and_c.test(bit_index::streaming_active))
		return true;
	if (s_and_c.test(bit_index::recording_active))
		return true;

	return false;
}

void update_visibility(std::bitset<3>)
{
	desktop_icons_visible.set_value(!should_be_hidden());
}

void setup()
{
	current_state.bind_value_changed(update_visibility);
	settings_state.bind_value_changed(update_visibility);
}
}
