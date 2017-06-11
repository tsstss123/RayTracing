#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

#include "util.h"
#include "light.h"
#include "boundingbox.h"

using std::vector;

class BoundingBox;

class Entity
{
public:
    virtual vec3f light_intersection(vec3f start, vec3f dir) = 0; //光线求交

};

class Renderable_Entity : public Entity
{
public:
	virtual vec3f normal_dir(vec3f point) = 0; //求法向量
    virtual vec3f render_point(vec3f ins, vec3f dir, std::vector<Renderable_Entity*> &entitys, std::vector<Light*> &lights) = 0;
    virtual vec3f get_reflex(vec3f ins, vec3f dir) = 0;
    virtual vec3f get_refraction(vec3f ins, vec3f dir) = 0;
    virtual void get_bounding(vec3f &bmin, vec3f &bmax) = 0;
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
    void get_bounding(vec3f &bmin, vec3f &bmax);
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
	void get_bounding(vec3f &bmin, vec3f &bmax);
};

#endif
