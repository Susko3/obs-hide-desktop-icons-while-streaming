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

#include "plugin-macros.generated.h"
#include "windows-main.h"

// FROM https://stackoverflow.com/a/53347282
#include <system_error>

void callback(obs_frontend_event event, void *data)
{
	(void)data;

	try {
		switch (event) {
		case OBS_FRONTEND_EVENT_STREAMING_STARTED:
			SetDesktopIconsVisible(false);
			break;

		case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
			SetDesktopIconsVisible(true);
			break;
		}
	} catch (const std::system_error &e) {
		blog(LOG_ERROR, "Error: %s", e.what());
	}
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	blog(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	obs_frontend_add_event_callback(callback, nullptr);
	return true;
}

void obs_module_unload()
{
	obs_frontend_remove_event_callback(callback, nullptr);

	blog(LOG_INFO, "plugin unloaded");
}
