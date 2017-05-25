#ifndef LIGHT_H
#define LIGHT_H

#include "util.h"

class Light
{
public:
	vec3f material;
public:
	Light(vec3f _material);
    virtual vec3f direction(vec3f point) = 0;
};

class PointLight : Light
{
public:
    vec3f start;

public:
	PointLight(vec3f _start, vec3f _material);
    vec3f direction(vec3f point);
};

class ParallelLight: Light
{
public:
    vec3f dir;

public:
	ParallelLight(vec3f _dir, vec3f _material);
    vec3f direction(vec3f point);
};

#endif
