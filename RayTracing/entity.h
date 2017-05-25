#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

#include "util.h"
#include "light.h"

class Entity
{
public:
    virtual vec3f light_intersection(vec3f start, vec3f dir) = 0; //光线求交

};

class BoundingBox_Entity : public Entity
{
public:
	vec3f min, max;
	BoundingBox_Entity *left, *right;

public:
	BoundingBox_Entity(vec3f _min, vec3f _max);
	vec3f get_center();
	float get_distance(BoundingBox_Entity *box);
	virtual vec3f light_intersection(vec3f start, vec3f dir);
};

class Renderable_Entity : public Entity
{
public:
	virtual vec3f normal_dir(vec3f point) = 0; //求法向量
    virtual vec3f render_point(vec3f ins, vec3f dir, std::vector<Renderable_Entity*> &entitys, std::vector<Light*> &lights) = 0;
    virtual vec3f get_reflex(vec3f ins, vec3f dir) = 0;
    virtual vec3f get_refraction(vec3f ins, vec3f dir) = 0;
    virtual BoundingBox_Entity get_bounding() = 0;
};

class Phong_Entity : public Renderable_Entity
{
public:
    float kd, ks, ka;
    int n;
	Color color;
	
public:
	void set_kd(float _kd)
	{
		kd = _kd;
	}
	float get_kd()
	{
		return kd;
	}
	void set_ks(float _ks)
	{
		ks = _ks;
	}
	float get_ks()
	{
		return ks;
	}
	void set_ka(float _ka)
	{
		ka = _ka;
	}
	float get_ka()
	{
		return ka;
	}
	void set_n(int _n)
	{
		n = _n;
	}
	int get_n()
	{
		return n;
	}
	virtual vec3f render_point(vec3f ins, vec3f dir, std::vector<Renderable_Entity*> &entitys, std::vector<Light*> &lights);
};

class Sphere_Phong : public Phong_Entity
{
public:
    vec3f center;
    float rad;

public:
	Sphere_Phong(vec3f _center, float _rad, Color _color);
    virtual vec3f light_intersection(vec3f start, vec3f dir);
    virtual vec3f normal_dir(vec3f point);
    virtual vec3f get_reflex(vec3f ins, vec3f dir);
    virtual vec3f get_refraction(vec3f ins, vec3f dir);
    BoundingBox_Entity get_bounding();
};

class Plane_Phong : public Phong_Entity
{
public:
	vec3f nvec;
    float d;

public:
	Plane_Phong(vec3f _n, float _d, Color _color);
    virtual vec3f light_intersection(vec3f start, vec3f dir);
    virtual vec3f normal_dir(vec3f point);
	virtual vec3f get_reflex(vec3f ins, vec3f dir);
	virtual vec3f get_refraction(vec3f ins, vec3f dir);
	BoundingBox_Entity get_bounding();
};

#endif
