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

#include "monitor_capture_manager.h"
#include "state/state.h"

namespace monitor_capture_manager
{

void update_from_current_scene();
bool connect_monitor_capture_signals(void *, obs_source_t *source);

bool is_monitor_capture(const obs_source_t *source)
{
	using namespace std::string_literals;
	return obs_source_get_unversioned_id(source) == "monitor_capture"s;
}

namespace signal_handler
{
	void activated(void *, calldata_t *)
	{
		state::update_bit(state::bit_index::display_capture, true);
	}

	void deactivated(void *, calldata_t *)
	{
		update_from_current_scene(); // we have been deactivated, but need to check the whole scene if any monitor source is still active
	}

	void destroyed(void *, calldata_t *)
	{
		update_from_current_scene(); // we have been destroyed/removed, but need to check the whole scene if any monitor source is still active
	}

	void source_created(void *, calldata_t *calldata)
	{
		const auto source = static_cast<obs_source_t *>(calldata_ptr(calldata, "source"));
		if (is_monitor_capture(source)) {
			connect_monitor_capture_signals(nullptr, source);
			state::update_bit(state::bit_index::display_capture, true); // let's assume the new display capture is active and visible be default.
		}
	}
}

bool connect_monitor_capture_signals(void *, obs_source_t *source)
{
	if (is_monitor_capture(source)) {
		const auto signal_handler = obs_source_get_signal_handler(source);
		signal_handler_connect(signal_handler, "activate", signal_handler::activated, nullptr);
		signal_handler_connect(signal_handler, "deactivate", signal_handler::deactivated, nullptr);
		signal_handler_connect(signal_handler, "destroy", signal_handler::destroyed, nullptr);
	}

	return true;
}

void hook_all_monitor_capture_signals()
{
	obs_enum_sources(connect_monitor_capture_signals, nullptr);
}

/**
 * \brief Hooks the global `source_create (ptr source)` signal
 *
 * See https://docs.obsproject.com/reference-core?highlight=source_create#core-obs-signals
 */
void hook_source_create_signal()
{
	const auto signal_handler = obs_get_signal_handler();
	signal_handler_connect(signal_handler, "source_create", signal_handler::source_created, nullptr);
}

namespace source_enumeration_processor
{
	struct processor_params {
		bool found;
		std::function<bool(obs_source_t *)> predicate;
	};

	void proc(obs_source_t *parent, obs_source_t *child, void *param)
	{
		const auto state = static_cast<processor_params *>(param);

		if (!state->found) {
			if (state->predicate(child)) {
				state->found = true;
			}
		}
	}

	/**
	 * \brief Checks the scene to see if there exsist at least one active source that matches the predicate.
	 * \param scene The scene to check
	 * \param predicate The predicate to match the sources to
	 * \return true if a matching source was found
	 */
	bool active_any_of(obs_source_t *scene, std::function<bool(obs_source_t *)> predicate)
	{
		processor_params state{false, std::move(predicate)};
		obs_source_enum_active_tree(scene, proc, &state);
		return state.found;
	}
}

void update_from_current_scene()
{
	const auto scene = obs_frontend_get_current_scene();

	state::update_bit(state::bit_index::display_capture, source_enumeration_processor::active_any_of(scene, is_monitor_capture));

	obs_source_release(scene);
}

void on_obs_frontend_event_finished_loading()
{
	state::settings_state.bind_value_changed([](const std::bitset<3> value) {
		if (value.test(state::bit_index::display_capture)) {
			hook_source_create_signal(); // only need to do this once, as the signal is global
			hook_all_monitor_capture_signals();
			update_from_current_scene();
		}
	}, true);
}

void on_obs_frontend_event_scene_changed()
{
	if (state::settings_state.get_value().test(state::bit_index::display_capture)) {
		hook_all_monitor_capture_signals();
		update_from_current_scene();
	}
}

}
