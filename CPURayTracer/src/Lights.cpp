#include "../header/Lights.h"

vec3 Directional::attenuation = vec3(1, 0, 0); // Directional Lights don't have attenuation.

void Light::Transform()
{
	pos = transf * pos;
}
