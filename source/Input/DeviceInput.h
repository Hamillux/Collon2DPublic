#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Utility/Core/Math/Vector2D.h"
#include "Utility/Core/Event.h"


/// <summary>
/// マウスやキーボードの入力状態を管理するクラス
/// </summary>
class DeviceInput
{
public:
	struct InputEvents {
		Event<> OnDragDropBegin;
		Event<> OnDragDropUpdate;
		Event<> OnDragDropEnd;
	};
	static InputEvents input_events;

	enum class ButtonState : uint8_t
	{
		Down = 1,		// 押されている
		Pressed = 2,	// 押された瞬間
		Up = 4,			// 離されている
		Released = 8,	// 離された瞬間

	};
	enum class WheelState : uint8_t
	{
		Up, Down, Stopped
	};
	static void Tick();
	static void ReleaseAllKey();

	/// <summary>
	/// 全ての入力をリセットする
	/// </summary>
	static void ResetAll();

	enum class Device {KEYBOARD, MOUSE};
	// キー入力状態を取得
	static ButtonState GetButtonState(const uint8_t dx_button_code, const Device device = Device::KEYBOARD);
	static bool IsActive(const uint8_t dx_button_code, const Device device = Device::KEYBOARD);	// Down or Pressed
	static bool IsPressed(const uint8_t dx_button_code, const Device device = Device::KEYBOARD);
	static bool IsReleased(const uint8_t dx_button_code, const Device device = Device::KEYBOARD);
	static bool IsUp(const uint8_t dx_button_code, const Device device = Device::KEYBOARD);
	static bool IsDown(const uint8_t dx_button_code, const Device device = Device::KEYBOARD);

	/// <summary>
	/// W:(Y=-1), S(Y=1), A(X=-1), D(X=1)として,
	/// Vector2D(A+D, W+S)を返す
	/// </summary>
	static Vector2D GetInputDir_WASD();

	static WheelState GetMouseWheelState();
	static int GetMouseWheelMove();
	static bool WheelIsUp();
	static bool WheelIsDown();
	static bool WheelIsStopped();

	static Vector2D GetMousePosition();
	static Vector2D GetMouseLastPosition();
	static Vector2D GetMouseDeltaPosition();
	static bool IsMouseMoved();

	// いずれかのキーが押されている
	static bool IsActiveAny(const Device device = Device::KEYBOARD);
private:
	static ButtonState GetNextButtonState(const ButtonState last_state, const bool is_active_now);
	static void UpdateKeyStates();
	static void UpdateMouseButtonStates();
	static void UpdateWheelState();
	static Vector2D CallDxLibGetMousePoint();

	static std::vector<ButtonState> key_states;
	static std::unordered_map<int, ButtonState> mouse_states;
	static WheelState wheel_state;

	static Vector2D _last_mouse_pos;
	static Vector2D _delta_mouse_pos;
	static Vector2D _current_mouse_pos;
	static bool _is_mouse_dragging;
	static bool _has_reset;
};