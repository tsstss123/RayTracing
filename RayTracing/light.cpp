#include "util.h"
#include "light.h"

Light::Light(vec3f _material)
{
	this->material = _material;
}

PointLight::PointLight(vec3f _start, vec3f _material):Light(_material)
{
	start = _start;
}

vec3f PointLight::direction(vec3f point)
{
    return point - start;
}

ParallelLight::ParallelLight(vec3f _dir, vec3f _material):Light(_material)
{
	dir = _dir;
}

vec3f ParallelLight::direction(vec3f point)
{
    return this->dir;
}
