#pragma once

enum MaterialType 
{
	Normal,
	Mirror // Mirror materials must have an ambient to weight reflection.
};

struct Material 
{
	// Blinn-Phong Properties.
	vec3 ambient = vec3(0.0);
	vec3 emission = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	uint64_t shininess = 0;

	MaterialType type = Normal;
};