#include "state.h"

namespace state
{

bool should_be_hidden()
{
	const auto settings = settings_state.get_value();
	const auto current = current_state.get_value();

	if (settings.test(bit_index::display_capture) && !current.test(bit_index::display_capture))
		return false;

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
