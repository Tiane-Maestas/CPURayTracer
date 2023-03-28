#pragma once

struct Material 
{
	vec3 ambient = vec3(0.0);
	vec3 emission = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	uint64_t shininess = 0;
};