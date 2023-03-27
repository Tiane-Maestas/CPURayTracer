#pragma once

struct Material 
{
	vec4 ambient = vec4(0.0);
	vec4 emission = vec4(0.0);
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);
	uint64_t shininess = 0;
};