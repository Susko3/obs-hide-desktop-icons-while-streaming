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

#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>

#include "Windows/windows-main.h"

// FROM https://stackoverflow.com/a/53347282
#include <system_error>

bool streamingActive = false;

/**
 * Whether desktop icons are visible, this is only a cached value and might not be 100% accurate wrt external
 * changes of icon visibility.
 */
bool desktopIconsVisible = true;

/**
 * @brief Updates desktop icons visibility in line with current streaming (and other) states.
 *
 * @sa streamingActive
 */
void updateDesktopIconsVisibility()
{
	// hide icons if streaming is active.
	bool newVisibility = !streamingActive;

	if (newVisibility == desktopIconsVisible)
		return;

	try {
		Windows::SetDesktopIconsVisible(newVisibility);
		desktopIconsVisible = newVisibility;
	} catch (const std::system_error &e) {
		obs_log(LOG_ERROR, "Error: %s", e.what());
	}
}

void callback(obs_frontend_event event, void *data)
{
	(void)data;

	switch (event) {
	case OBS_FRONTEND_EVENT_STREAMING_STARTED:
	case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
		streamingActive = event == OBS_FRONTEND_EVENT_STREAMING_STARTED;
		updateDesktopIconsVisibility();
		break;
	}
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	try {
		desktopIconsVisible = Windows::GetDesktopIconsVisible();
	} catch (const std::system_error &e) {
		obs_log(LOG_ERROR, "Error when fetching desktop icon visibility. Assuming it's visible. Error: %s", e.what());
	}

	obs_frontend_add_event_callback(callback, nullptr);
	return true;
}

void obs_module_unload()
{
	obs_frontend_remove_event_callback(callback, nullptr);

	obs_log(LOG_INFO, "plugin unloaded");
}
