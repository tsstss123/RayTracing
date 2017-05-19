#include <cmath>
#include <algorithm>
#include <vector>

#include "entity.h"

vec3f Entity::get_reflex(vec3f ins, vec3f dir)
{
    return vec3f();
}

vec3f Entity::get_refraction(vec3f ins, vec3f dir)
{
    return vec3f();
}
float Entity::get_weight_rx()
{
	return 0.0f;
}
float Entity::get_weight_rt()
{
	return 0.0f;
}
Sphere_Phong::Sphere_Phong(vec3f _center, float _rad, Color _color)
{
	this->center = _center;
	this->rad = _rad;
	this->color = _color;
}
vec3f Sphere_Phong::light_intersection(vec3f start, vec3f dir)
{
    dir = dir.norm();
    vec3f ptos = start - this->center;
    float B = dir * ptos * 2;
    float C = this->center * this->center - this->rad * this->rad;
    float delta = B * B - C * 4;
    if(delta <= 0)
        return vec3f();
    float t = std::min(-B + std::sqrt(delta), -B + std::sqrt(delta)) / 2;
    return start + dir * t;
}

vec3f Sphere_Phong::normal_dir(vec3f point)
{
    return (point - this->center).norm();
}

vec3f Sphere_Phong::render_point(vec3f ins, vec3f dir, std::vector<Entity*> &entitys, std::vector<Light*> &lights)
{
	float I = 0;
	vec3f N = this->normal_dir(ins);
	vec3f V = (vec3f() - dir).norm();
	for (auto light : lights)
	{
		vec3f L = light->direction(ins).norm();
		vec3f H = (L + V).norm();
		if (N * L > 0)
		{
			I += N * L * kd;
		}
		if (N * H > 0)
		{
			I += std::pow(N * H, n) * ks;
		}
	}
	return this->color.to_vec() * I;
}

vec3f Sphere_Phong::get_reflex(vec3f ins, vec3f dir)
{
	vec3f N = this->normal_dir(ins);
	vec3f L = vec3f() - dir;
	vec3f R = (N * (N * L) * 2 - L).norm();
	return R;
}

vec3f Sphere_Phong::get_refraction(vec3f ins, vec3f dir)
{
	return vec3f();
}

vec3f Plane_Phong::light_intersection(vec3f start, vec3f dir)
{
    return vec3f();
    //TODO
}

vec3f Plane_Phong::normal_dir(vec3f point)
{
    return vec3f();
    //TODO
}

vec3f Plane_Phong::render_point(vec3f ins, vec3f dir, std::vector<Entity*>& entitys, std::vector<Light*>& lights)
{
	return vec3f();
}
