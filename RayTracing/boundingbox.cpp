#include <algorithm>
#include "boundingbox.h"

using std::min;
using std::max;

BoundingBox::BoundingBox(vec3f _min, vec3f _max, Renderable_Entity* _entity = nullptr)
{
	this->bmin = _min;
	this->bmax = _max;
	this->entity = _entity;
}

BoundingBox::BoundingBox(vector<BoundingBox*> &boxes)
{
	assert(boxes.size() > 0);
	this->children = boxes;
	float xmin = double_inf, ymin = double_inf, zmin = double_inf;
	float xmax = -double_inf, ymax = -double_inf, zmax = -double_inf;
	for(auto box : boxes)
	{
		xmin = min(xmin, box->bmin.x);
		ymin = min(ymin, box->bmin.y);
		zmin = min(zmin, box->bmin.z);
		
		xmax = max(xmax, box->bmax.x);
		ymax = max(ymax, box->bmax.y);
		zmax = max(zmax, box->bmax.z);
	}
	this->bmin = vec3f(xmin, ymin, zmin);
	this->bmax = vec3f(xmax, ymax, zmax);
	this->entity = nullptr;
}

vec3f BoundingBox::get_center()
{
	return (this->bmin + this->bmax) * 0.5;
}

float BoundingBox::get_distance(BoundingBox *box)
{
	return (this->get_center() - box->get_center()).length();
}

bool BoundingBox::rayintersects(vec3f start, vec3f dir)
{
	vec3f ptOnPlane; //射线与包围盒某面的交点

	const vec3f& origin = start; //射线起始点

	float t;

	//分别判断射线与各面的相交情况

	//判断射线与包围盒x轴方向的面是否有交点
	if (dir.x != 0.f) //射线x轴方向分量不为0 若射线方向矢量的x轴分量为0，射线不可能经过包围盒朝x轴方向的两个面
	{
		/*
		使用射线与平面相交的公式求交点
		*/
		if (dir.x > 0)//若射线沿x轴正方向偏移
			t = (bmin.x - origin.x) / dir.x;
		else  //射线沿x轴负方向偏移
			t = (bmax.x - origin.x) / dir.x;

		if (t > 0.f) //t>0时则射线与平面相交
		{
			ptOnPlane = origin + dir * t; //计算交点坐标
										  //判断交点是否在当前面内
			if (bmin.y < ptOnPlane.y && ptOnPlane.y < bmax.y && bmin.z < ptOnPlane.z && ptOnPlane.z < bmax.z)
			{
				return true; //射线与包围盒有交点
			}
		}
	}

	//若射线沿y轴方向有分量 判断是否与包围盒y轴方向有交点
	if (dir.y != 0.f)
	{
		if (dir.y > 0)
			t = (bmin.y - origin.y) / dir.y;
		else
			t = (bmax.y - origin.y) / dir.y;

		if (t > 0.f)
		{
			ptOnPlane = origin + dir * t;

			if (bmin.z < ptOnPlane.z && ptOnPlane.z < bmax.z && bmin.x < ptOnPlane.x && ptOnPlane.x < bmax.x)
			{
				return true;
			}
		}
	}

	//若射线沿z轴方向有分量 判断是否与包围盒y轴方向有交点
	if (dir.z != 0.f)
	{
		if (dir.z > 0)
			t = (bmin.z - origin.z) / dir.z;
		else
			t = (bmax.z - origin.z) / dir.z;

		if (t > 0.f)
		{
			ptOnPlane = origin + dir * t;

			if (bmin.x < ptOnPlane.x && ptOnPlane.x < bmax.x && bmin.y < ptOnPlane.y && ptOnPlane.y < bmax.y)
			{
				return true;
			}
		}
	}

	return false;
}

Renderable_Entity* BoundingBox::get_lighting_entity(vec3f start, vec3f dir)
{
	if(this->entity != nullptr)
	{
		vec3f point = this->entity->light_intersection(start, dir);
		if(point.is_zero())
		{
			return nullptr;
		}
		else
		{
			return this->entity;
		}
	}
	if (bmin != bmax) //spec box
	{
		if (!this->rayintersects(start, dir))
		{
			return nullptr;
		}
	}
	Renderable_Entity* result = nullptr;
	double distance = 0;
	for(auto box: children)
	{
		Renderable_Entity* ins = box->get_lighting_entity(start, dir);
		if(ins != nullptr)
		{
			vec3f ins_point = ins->light_intersection(start, dir);
			assert(!ins_point.is_zero());
			double ins_dist = (ins_point - start).length();
			if(result == nullptr || distance > ins_dist)
			{
				result = ins;
				distance = ins_dist;
			}
		}
	}
	return result;
}
