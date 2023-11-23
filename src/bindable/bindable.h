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
	void set_value(T value);
	void bind_value_changed(value_changed_handler handler, bool run_once_immediately = false);
	void unbind_all();
};

template<typename T> T bindable<T>::get_value()
{
	return value_;
}

template<typename T> void bindable<T>::set_value(T value)
{
	if (value_ == value)
		return;

	value_ = value;
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
