#include "MathUtils.h"
#include <imgui.h>

const ImVec2& operator+(const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2(v1.x + v2.x, v1.y + v2.y);
}

const ImVec2& operator-(const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2(v1.x - v2.x, v1.y - v2.y);
}

const ImVec2& operator*(const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2(v1.x * v2.x, v1.y * v2.y);
}

const ImVec2& operator*(const ImVec2& v, const float a)
{
	return ImVec2(v.x * a, v.y * a);
}

const ImVec2& operator*(const float a, const ImVec2& v)
{
	return v * a;
}

const ImVec2& operator/(const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2(v1.x / v2.x, v1.y / v2.y);
}

const ImVec2& operator+(const ImVec2&& v1, const ImVec2&& v2)
{
	return ImVec2(v1.x + v2.x, v1.y + v2.y);
}

const ImVec2& operator*(const ImVec2&& v1, const ImVec2&& v2)
{
	return ImVec2(v1.x * v2.x, v1.y * v2.y);
}