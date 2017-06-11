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
	vec3f ptOnPlane; //�������Χ��ĳ��Ľ���

	const vec3f& origin = start; //������ʼ��

	float t;

	//�ֱ��ж������������ཻ���

	//�ж��������Χ��x�᷽������Ƿ��н���
	if (dir.x != 0.f) //����x�᷽�������Ϊ0 �����߷���ʸ����x�����Ϊ0�����߲����ܾ�����Χ�г�x�᷽���������
	{
		/*
		ʹ��������ƽ���ཻ�Ĺ�ʽ�󽻵�
		*/
		if (dir.x > 0)//��������x��������ƫ��
			t = (bmin.x - origin.x) / dir.x;
		else  //������x�Ḻ����ƫ��
			t = (bmax.x - origin.x) / dir.x;

		if (t > 0.f) //t>0ʱ��������ƽ���ཻ
		{
			ptOnPlane = origin + dir * t; //���㽻������
										  //�жϽ����Ƿ��ڵ�ǰ����
			if (bmin.y < ptOnPlane.y && ptOnPlane.y < bmax.y && bmin.z < ptOnPlane.z && ptOnPlane.z < bmax.z)
			{
				return true; //�������Χ���н���
			}
		}
	}

	//��������y�᷽���з��� �ж��Ƿ����Χ��y�᷽���н���
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

	//��������z�᷽���з��� �ж��Ƿ����Χ��y�᷽���н���
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
