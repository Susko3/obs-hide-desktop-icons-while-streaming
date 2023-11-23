#include "settings.h"
#include "state/state.h"
#include <bitset>
#include <filesystem>
#include <obs-frontend-api.h>
#include <obs-module.h>

// code for the settings item under OBS → Tools

namespace settings
{

const char *config_file_string = "obs-hide-desktop-icons-while-streaming.json";

namespace property_id
{
	const auto hide_when = "text1";
	const auto explanation_text = "explanation_text";
	const auto streaming_active = "streaming_active";
	const auto recording_active = "recording_active";
	const auto display_capture = "display_capture";
}

namespace property_name
{
	const auto streaming_active = "Streaming is active";
	const auto recording_active = "Recording is active";
	const auto display_capture = "Display Capture source is visible "; // space so it looks nice with the long description
}

namespace info_text
{
	const auto hide_when = "Hide desktop icons when:";
	const auto not_available = "Hiding when a display capture source is visible is not yet implemented. Check back in a future version of the plugin!";
	const auto always_visible = "Desktop icons will always be visible.";
	const auto streaming = "Desktop icons will be hidden when OBS is streaming.";
	const auto recording = "Desktop icons will be hidden when OBS is recording.";
	const auto recording_or_streaming = "Desktop icons will be hidden when OBS is streaming <b>or</b> recording.";
	const auto display_capture = "Desktop icons will be hidden when a Display Capture Source is visible in the active Scene.";
	const auto recording_and_display_capture = "Desktop icons will be hidden when OBS is recording <b>and</b> a Display Capture Source is visible in the active Scene.";
	const auto streaming_and_display_capture = "Desktop icons will be hidden when OBS is streaming <b>and</b> a Display Capture Source is visible in the active Scene.";
	const auto recording_or_streaming_and_display_capture =
		"Desktop icons will be hidden when OBS is streaming <b>or</b> recording, <b>and</b> a Display Capture Source is visible in the active Scene.";

	const auto description_text_list = {
		always_visible,
		streaming,
		recording,
		recording_or_streaming,
		display_capture,
		streaming_and_display_capture,
		recording_and_display_capture,
		recording_or_streaming_and_display_capture
	};
}

std::bitset<3> stored_settings; // for text visibility only, actual settings should only apply when saved

bool update_stored_settings(obs_data_t *settings, bool update_bindable)
{
	std::bitset<3> new_settings;
	new_settings[0] = obs_data_get_bool(settings, property_id::streaming_active);
	new_settings[1] = obs_data_get_bool(settings, property_id::recording_active);
	new_settings[2] = obs_data_get_bool(settings, property_id::display_capture);

	if (stored_settings != new_settings) {
		stored_settings = new_settings;
		return true;
	}

	if (update_bindable) {
		state::settings_state.set_value(stored_settings);
	}

	return false;
}

const char *dummy_source_get_name(void *)
{
	return obs_module_text("ColorSource");
}

void dummy_source_update(void *, obs_data_t *settings)
{
	update_stored_settings(settings, true);
}

void *dummy_source_create(obs_data_t *settings, obs_source_t *source)
{
	auto lol = new int(42);
	return lol;
}

void dummy_source_destroy(void *data)
{
	auto lol = static_cast<int *>(data);
	delete lol;
}

void update_props_explanation_text(obs_properties_t *props)
{
	const auto idx = info_text::description_text_list.begin() + stored_settings.to_ullong();
	assert(idx < info_text::description_text_list.end());
	obs_property_set_description(obs_properties_get(props, property_id::explanation_text), *idx);
}

obs_source_t *dummy;

bool on_any_changed(obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	const bool changed = update_stored_settings(settings, false);
	if (changed) {
		update_props_explanation_text(props);
	}
	return changed;
}

obs_properties_t *dummy_source_properties(void *)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_set_flags(props, OBS_PROPERTIES_DEFER_UPDATE);

	obs_properties_add_text(props, property_id::hide_when, info_text::hide_when, OBS_TEXT_INFO);
	const auto streaming_active = obs_properties_add_bool(props, property_id::streaming_active, property_name::streaming_active);
	const auto recording_active = obs_properties_add_bool(props, property_id::recording_active, property_name::recording_active);
	const auto display_capture = obs_properties_add_bool(props, property_id::display_capture, property_name::display_capture);
	obs_property_set_enabled(display_capture, false);
	obs_property_set_long_description(display_capture, info_text::not_available);

	const auto explanation_text = obs_properties_add_text(props, property_id::explanation_text, "explanation_text", OBS_TEXT_INFO);
	obs_property_text_set_info_word_wrap(explanation_text, false); // disable weird behaviour when toggling settings.

	for (const auto p : {streaming_active, recording_active, display_capture})
		obs_property_set_modified_callback(p, on_any_changed);

	update_props_explanation_text(props);

	return props;
}

void dummy_source_defaults(obs_data_t *settings)
{
	obs_data_set_default_bool(settings, property_id::streaming_active, true);
	obs_data_set_default_bool(settings, property_id::recording_active, false);
	obs_data_set_default_bool(settings, property_id::display_capture, false);
	obs_data_set_bool(settings, property_id::display_capture, false);
}

const char *dummy_source_name = "obs-hide-desktop-icons-while-streaming_dummy-source";

obs_source_info dummy_source_definition = {
	.id = dummy_source_name,
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_CAP_DISABLED,
	.get_name = dummy_source_get_name,
	.create = dummy_source_create,
	.destroy = dummy_source_destroy,
	.get_defaults = dummy_source_defaults,
	.get_properties = dummy_source_properties,
	.update = dummy_source_update, // optional
};

void open_settings(void *)
{
	obs_frontend_open_source_properties(dummy);
}

void on_obs_module_load()
{
	obs_register_source(&dummy_source_definition);
}

void on_obs_frontend_event_finished_loading()
{
	const auto folder_path = obs_module_config_path(nullptr);
	std::filesystem::create_directory(folder_path);
	bfree(folder_path);

	const auto json_path = obs_module_config_path(config_file_string);
	const auto settings = obs_data_create_from_json_file(json_path);
	dummy = obs_source_create(dummy_source_name, "Hide desktop icons while streaming", settings, nullptr);
	update_stored_settings(obs_source_get_settings(dummy), true); // update settings after the dummy source has had a change to fill in the defaults.
	obs_data_release(settings);
	bfree(json_path);

	obs_frontend_add_tools_menu_item("Auto Hide &Desktop Icons", &open_settings, nullptr);
}

void on_obs_frontend_event_exit()
{
	const auto json_path = obs_module_config_path(config_file_string);
	const auto settings = obs_source_get_settings(dummy);
	obs_data_save_json(settings, json_path);
	obs_data_release(settings);
	bfree(json_path);

	obs_source_release(dummy);
}

}
