#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <vector>

#include "util.h"
#include "entity.h"

using std::vector;

class Renderable_Entity;

class BoundingBox
{
public:
	vec3f bmin, bmax; //special box is bmin == bmax
	vector<BoundingBox*> children;
	Renderable_Entity* entity; //if it's not null, it's a entity

public:
	BoundingBox(vec3f _min, vec3f _max, Renderable_Entity* _entity);
	BoundingBox(vector<BoundingBox*> &boxes);
	vec3f get_center();
	float get_distance(BoundingBox *box);
	bool rayintersects(vec3f start, vec3f dir);
	Renderable_Entity* get_lighting_entity(vec3f start, vec3f dir);
	
};

#endif
