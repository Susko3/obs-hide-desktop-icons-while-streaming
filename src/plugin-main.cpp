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

#include "settings/settings.h"
#include "state/state.h"
#include "windows/windows-main.h"
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>
#include <system_error>

void update(const size_t index, const bool value)
{
	auto val = state::current_state.get_value();
	val[index] = value;
	state::current_state.set_value(val);
}

void callback(const obs_frontend_event event, void *)
{
	(void)data;

	switch (event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING:
		settings::on_obs_frontend_event_finished_loading();
		break;

	case OBS_FRONTEND_EVENT_STREAMING_STARTING:
		update(state::bit_index::streaming_active, true);
		break;
	case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
		update(state::bit_index::streaming_active, false);
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STARTING:
		update(state::bit_index::recording_active, true);
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
		update(state::bit_index::recording_active, false);
		break;

	case OBS_FRONTEND_EVENT_EXIT:
		settings::on_obs_frontend_event_exit();
		break;
	}
}

void set_icons_visible(const bool value)
{
	try {
		Windows::SetDesktopIconsVisible(value);
	} catch (const std::system_error &e) {
		obs_log(LOG_ERROR, "Error: %s", e.what());
	}
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load()
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	state::setup();
	settings::on_obs_module_load();

	try {
		state::desktop_icons_visible.set_value(Windows::GetDesktopIconsVisible());
	} catch (const std::system_error &e) {
		obs_log(LOG_ERROR, "Error when fetching desktop icon visibility. Assuming it's visible. Error: %s", e.what());
		state::desktop_icons_visible.set_value(true);
	}

	state::desktop_icons_visible.bind_value_changed(set_icons_visible);

	obs_frontend_add_event_callback(callback, nullptr);
	return true;
}

void obs_module_unload()
{
	obs_frontend_remove_event_callback(callback, nullptr);

	state::desktop_icons_visible.unbind_all();
	state::desktop_icons_visible.set_value(false);

	obs_log(LOG_INFO, "plugin unloaded");
}
