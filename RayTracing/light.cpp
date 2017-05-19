#include "util.h"
#include "light.h"

PointLight::PointLight(vec3f _start)
{
	start = _start;
}

vec3f PointLight::direction(vec3f point)
{
    return point - start;
}

ParallelLight::ParallelLight(vec3f _dir)
{
	dir = _dir;
}

vec3f ParallelLight::direction(vec3f point)
{
    return this->dir;
}