#include "MdGameIcon.h"
#include "MdImageFile.h"

void MdGameIcon::CalcUV(float& u_0, float& v_0, float& u_1, float& v_1) const
{
	const float image_height = static_cast<float>(MdImageFile::GetHeight(image_id));
	const float image_width = static_cast<float>(MdImageFile::GetWidth(image_id));

	u_0 = static_cast<float>(left) / image_width;
	v_0 = static_cast<float>(top) / image_height;

	u_1 = static_cast<float>(left + width) / image_width;
	v_1 = static_cast<float>(top + height) / image_height;
}