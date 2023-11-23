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
#include <functional>
#include <vector>

template<typename T>
class bindable {
public:
	using value_changed_handler = std::function<void(T)>;

private:
	T value_;
	std::vector<value_changed_handler> value_changed_handlers_;

public:
	explicit bindable(T value)
		: value_(value) {}

	T get_value();
	/**
	 * \brief Sets the value of the bindable, propagating it to bindings if it changed.
	 * \param value The new value
	 * \return Whether the underlying value actually changed
	 */
	bool set_value(T value);

	/**
	 * \brief Triggers a value change event to be fired to all handlers.
	 */
	void trigger_change();
	void bind_value_changed(value_changed_handler handler, bool run_once_immediately = false);
	void unbind_all();
};

template<typename T> T bindable<T>::get_value()
{
	return value_;
}

template<typename T> bool bindable<T>::set_value(T value)
{
	if (value_ == value)
		return false;

	value_ = value;
	trigger_change();
	return true;
}

template<typename T> void bindable<T>::trigger_change()
{
	for (const auto &f : value_changed_handlers_) {
		f(value_);
	}
}

template<typename T> void bindable<T>::bind_value_changed(value_changed_handler handler, bool run_once_immediately)
{
	value_changed_handlers_.push_back(handler);

	const auto a1 = handler.template target<void(*)(T)>();

	if (run_once_immediately) {
		handler(value_);
	}
}

template<typename T> void bindable<T>::unbind_all()
{
	value_changed_handlers_.clear();
}
