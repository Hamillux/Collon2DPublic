#include "HitResult.h"

HitResult HitResult::GetInverted() const
{
    HitResult inverted_result;

	// Œ³‚ÌŒ‹‰Ê‚Æ”½“]‚µ‚½Œ‹‰Ê‚Å‹¤’Ê‚Ì€–Ú
    inverted_result.has_hit = has_hit;
	inverted_result.collision_type = collision_type;
	inverted_result.total_push_back_distance = total_push_back_distance;

	// ”½“]‚Å“ü‚ê‘Ö‚¦‚é€–Ú
	inverted_result.self_collider = other_collider;
	inverted_result.other_collider = self_collider;
	inverted_result.normal_from_other = normal_from_other * (-1.f);

	return inverted_result;
}