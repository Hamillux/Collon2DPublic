#include "DxLibExtension.h"

DxLibScreenCapture::DxLibScreenCapture(const int screen_x, const int screen_y, const int use_alpha)
	: _screen_x(screen_x), _screen_y(screen_y)
{
	_handle = DxLib::MakeScreen(screen_x, screen_y, use_alpha);
}

DxLibScreenCapture::~DxLibScreenCapture()
{
	DeleteGraph(_handle);
}

void DxLibScreenCapture::SaveToPNG(const tstring& file_name) const
{
	const int last_screen = DxLib::GetDrawScreen();

	DxLib::SetDrawScreen(_handle);
	DxLib::SaveDrawScreen(0, 0, _screen_x, _screen_y, file_name.c_str());

	DxLib::SetDrawScreen(last_screen);
}
