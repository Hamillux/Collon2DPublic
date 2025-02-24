#include "DeviceInput.h"
#include "Core.h"
#include <DxLib.h>
#include <imgui.h>

namespace
{
	constexpr const uint16_t KEY_BUFFER_SIZE = 256;
	constexpr const float DRAGDROP_MOUSEMOVE_THRESHOLD = 2.f;
}
std::vector<DeviceInput::ButtonState> DeviceInput::key_states(KEY_BUFFER_SIZE, DeviceInput::ButtonState::Up);

std::unordered_map<int, DeviceInput::ButtonState> DeviceInput::mouse_states(
	{
		{MOUSE_INPUT_LEFT, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_RIGHT, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_MIDDLE, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_4, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_5, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_6, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_7, DeviceInput::ButtonState::Up},
		{MOUSE_INPUT_8, DeviceInput::ButtonState::Up}
	});

DeviceInput::WheelState DeviceInput::wheel_state = DeviceInput::WheelState::Stopped;

DeviceInput::InputEvents DeviceInput::input_events = DeviceInput::InputEvents();
Vector2D DeviceInput::_last_mouse_pos = DeviceInput::CallDxLibGetMousePoint();
Vector2D DeviceInput::_current_mouse_pos = DeviceInput::CallDxLibGetMousePoint();
Vector2D DeviceInput::_delta_mouse_pos = Vector2D();
bool DeviceInput::_is_mouse_dragging = false;
bool DeviceInput::_has_reset = false;

void DeviceInput::Tick()
{
	if (_has_reset)
	{
		if (DxLib::CheckHitKeyAll() == 0)
		{
			_has_reset = false;
		}
		return;
	}

	UpdateKeyStates();
	UpdateMouseButtonStates();
	UpdateWheelState();

	_last_mouse_pos = _current_mouse_pos;
	_current_mouse_pos = CallDxLibGetMousePoint();
	_delta_mouse_pos = _current_mouse_pos - _last_mouse_pos;
	if (_delta_mouse_pos.Length() < EPSIRON)
	{
		_delta_mouse_pos.x = 0.f;
		_delta_mouse_pos.y = 0.f;
	}

	if (_is_mouse_dragging)
	{
		if (IsDown(MOUSE_INPUT_LEFT, Device::MOUSE))
		{
			input_events.OnDragDropUpdate.Dispatch();
		}
		else if (IsReleased(MOUSE_INPUT_LEFT, Device::MOUSE))
		{
			_is_mouse_dragging = false;
			input_events.OnDragDropEnd.Dispatch();
		}
	}
	else
	{
		if (IsActive(MOUSE_INPUT_LEFT, Device::MOUSE) && _delta_mouse_pos.Length() > DRAGDROP_MOUSEMOVE_THRESHOLD)
		{
			_is_mouse_dragging = true;
			input_events.OnDragDropBegin.Dispatch();
		}
	}
}

void DeviceInput::ReleaseAllKey()
{
	for (auto& state : key_states)
	{
		state = ButtonState::Up;
	}
}

void DeviceInput::ResetAll()
{
	ReleaseAllKey();
	input_events = InputEvents();
	_has_reset = true;
}

DeviceInput::ButtonState DeviceInput::GetButtonState(const uint8_t dx_button_code, const Device device)
{
	if (device == Device::KEYBOARD)
	{
		return key_states.at(dx_button_code);
	}
	else if (device == Device::MOUSE)
	{
		return mouse_states.at(dx_button_code);
	}
	return ButtonState::Up;
}

bool DeviceInput::IsActive(const uint8_t dx_button_code, const Device device)
{
	return IsPressed(dx_button_code, device) || IsDown(dx_button_code, device);

	if (device == Device::KEYBOARD)
	{
		bool is_active =
			key_states.at(dx_button_code) == ButtonState::Down ||
			key_states.at(dx_button_code) == ButtonState::Pressed;
		return is_active;
	}
	else if (device == Device::MOUSE)
	{
		bool is_active =
			key_states.at(dx_button_code) == ButtonState::Down ||
			key_states.at(dx_button_code) == ButtonState::Pressed;
		return is_active;
	}
}

bool DeviceInput::IsPressed(const uint8_t dx_button_code, const Device device)
{
	if (device == Device::KEYBOARD)
	{
		return key_states.at(dx_button_code) == ButtonState::Pressed;
	}
	else if (device == Device::MOUSE)
	{
		return mouse_states.at(dx_button_code) == ButtonState::Pressed;
	}

	return false;
}

bool DeviceInput::IsReleased(const uint8_t dx_button_code, const Device device)
{
	if (device == Device::KEYBOARD)
	{
		return key_states.at(dx_button_code) == ButtonState::Released;
	}
	else if (device == Device::MOUSE)
	{
		return mouse_states.at(dx_button_code) == ButtonState::Released;
	}

	return false;
}

bool DeviceInput::IsUp(const uint8_t dx_button_code, const Device device)
{
	if (device == Device::KEYBOARD)
	{
		return key_states.at(dx_button_code) == ButtonState::Up;
	}
	else if (device == Device::MOUSE)
	{
		return mouse_states.at(dx_button_code) == ButtonState::Up;
	}

	return true;
}

bool DeviceInput::IsDown(const uint8_t dx_button_code, const Device device)
{
	if (device == Device::KEYBOARD)
	{
		return key_states.at(dx_button_code) == ButtonState::Down;
	}
	else if (device == Device::MOUSE)
	{
		return mouse_states.at(dx_button_code) == ButtonState::Down;
	}

	return false;
}

Vector2D DeviceInput::GetInputDir_WASD()
{
	const bool W = IsActive(KEY_INPUT_W);
	const bool A = IsActive(KEY_INPUT_A);
	const bool S = IsActive(KEY_INPUT_S);
	const bool D = IsActive(KEY_INPUT_D);

	return Vector2D(D - A, S - W);
}

DeviceInput::WheelState DeviceInput::GetMouseWheelState()
{
	return wheel_state;
}

int DeviceInput::GetMouseWheelMove()
{
	switch (GetMouseWheelState())
	{
	case WheelState::Up: return 1;
	case WheelState::Down: return -1;
	}

	return 0;
}

bool DeviceInput::WheelIsUp()
{
	return wheel_state == WheelState::Up;
}

bool DeviceInput::WheelIsDown()
{
	return wheel_state == WheelState::Down;
}

bool DeviceInput::WheelIsStopped()
{
	return wheel_state == WheelState::Stopped;
}

bool DeviceInput::IsMouseMoved()
{
	return !_delta_mouse_pos.IsZeroVector();
}

Vector2D DeviceInput::GetMousePosition()
{
	return _current_mouse_pos;
}

Vector2D DeviceInput::GetMouseLastPosition()
{
	return _last_mouse_pos;
}

Vector2D DeviceInput::GetMouseDeltaPosition()
{
	return _delta_mouse_pos;
}

bool DeviceInput::IsActiveAny(const Device device)
{
	if (device == Device::KEYBOARD)
	{
		for (const auto& state : key_states)
		{
			if (state == ButtonState::Down || state == ButtonState::Pressed)
			{
				return true;
			}
		}
	}
	else if (device == Device::MOUSE)
	{
		for (const auto& button_state_pair : mouse_states)
		{
			const ButtonState& state = button_state_pair.second;
			if (state == ButtonState::Down || state == ButtonState::Pressed)
			{
				return true;
			}
		}
	}
	return false;
}

DeviceInput::ButtonState DeviceInput::GetNextButtonState(const ButtonState last_state, const bool button_is_active_now)
{
	// Last: Active -> Current: InActive  => Released
	if ((last_state == ButtonState::Pressed || last_state == ButtonState::Down) && !button_is_active_now)
	{
		return ButtonState::Released;
	}

	// Last: InActive -> Current: Active  => Pressed
	if ((last_state == ButtonState::Released || last_state == ButtonState::Up) && button_is_active_now)
	{
		return ButtonState::Pressed;
	}

	// Last: Released -> Current: InActive  => Up
	if (last_state == ButtonState::Released && !button_is_active_now)
	{
		return ButtonState::Up;
	}

	// Last: Pressed -> Current: Active  => Down
	if (last_state == ButtonState::Pressed && button_is_active_now)
	{
		return ButtonState::Down;
	}

	return last_state;
}

void DeviceInput::UpdateKeyStates()
{
	char key_buf[KEY_BUFFER_SIZE];
	GetHitKeyStateAll(key_buf);

	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		const ButtonState last_key_state = key_states.at(i);
		const bool key_is_active_now = (key_buf[i] == 1);
		key_states.at(i) = GetNextButtonState(last_key_state, key_is_active_now);
	}
}

void DeviceInput::UpdateMouseButtonStates()
{
	// MOUSE_INPUT_X との論理積が0でなければマウスボタンXは押されている
	const int mouse_input = GetMouseInput();

	for (auto& button_state_pair : mouse_states)
	{
		const ButtonState last_mouse_state = button_state_pair.second;
		const bool button_is_active_now = ((mouse_input & button_state_pair.first) != 0);
		button_state_pair.second = GetNextButtonState(last_mouse_state, button_is_active_now);
	}
}

void DeviceInput::UpdateWheelState()
{
	int rot = GetMouseWheelRotVol();

	if (rot > 0)
	{
		wheel_state = WheelState::Up;
	}
	else if (rot < 0)
	{
		wheel_state = WheelState::Down;
	}
	else
	{
		wheel_state = WheelState::Stopped;
	}
}

Vector2D DeviceInput::CallDxLibGetMousePoint()
{
	int mx, my;
	DxLib::GetMousePoint(&mx, &my);
	return Vector2D(mx, my);
}
