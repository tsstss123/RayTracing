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
	float C = ptos * ptos - this->rad * this->rad;
    float delta = B * B - C * 4;
    if(delta <= 0)
        return vec3f();
    float tmin = std::min(-B - std::sqrt(delta), -B + std::sqrt(delta)) / 2;
	float tmax = std::max(-B - std::sqrt(delta), -B + std::sqrt(delta)) / 2;
	if (tmin > eps)
	{
		return start + dir * tmin;
	}
	else
	{
		return vec3f();
	}
	
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
		bool is_shadow = false;
		vec3f L = light->direction(ins).norm();
		
		for (auto entity : entitys)
		{
			if (entity == this)
			{
				continue;
			}
			if (!entity->light_intersection(ins, vec3f() - L).is_zero())
			{
				is_shadow = true;
				break;
			}
		}
		if (is_shadow)
		{
			continue;
		}

		vec3f R = this->get_reflex(ins, L);
		if (N * (vec3f() - L) > 0)
		{
			I += N * (vec3f() - L) * kd;
		}
		if (V * R > 0)
		{
			I += std::pow(V * R, n) * ks;
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
