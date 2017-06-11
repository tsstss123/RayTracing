//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <random>

#include "util.h"
#include "light.h"
#include "entity.h"

using std::vector;
using std::stack;
using std::shared_ptr;
using std::string;

const int BUFFERSIZE = 2000 * 2000;
const int maxDepth = 5;

unsigned char BufferR[BUFFERSIZE], BufferG[BUFFERSIZE], BufferB[BUFFERSIZE];
void cleanframe()
{
    memset(BufferR, 0, sizeof(BufferR));
    memset(BufferG, 0, sizeof(BufferG));
    memset(BufferB, 0, sizeof(BufferB));
}
unsigned char resize(float v)
{
	if (v > 255)return 255;
	if (v < 0) return 0;
	return (unsigned char)v;
}
void output(int px, int py, string fname)
{
	unsigned char head_buf[100] = { 66,77,54,8,7,0,0,0,0,0,54,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,1,0,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	FILE *fout = fopen(fname.c_str(), "wb");
	int *xlenp = (int *)&head_buf[18], *ylenp = (int *)&head_buf[22];
	int off = (4 - (px * 3 % 4)) % 4;
	unsigned char Zero[] = { 0,0,0,0 };
	*xlenp = px; *ylenp = py;
	fwrite(head_buf, 54, 1, fout);
	for (int i = 0; i < px * py; ++i)
	{
		fwrite(&BufferB[i], 1, 1, fout);
		fwrite(&BufferG[i], 1, 1, fout);
		fwrite(&BufferR[i], 1, 1, fout);
		if (i && i % px == px - 1 && off)
		{
			fwrite(Zero, off, 1, fout);
		}
	}
	fclose(fout);
}

vector<Renderable_Entity*> entitys;
vector<Light*> lights;
vector<BoundingBox*> BoundingBoxes;

vec3f RayTracing(vec3f start, vec3f dir, int depth)
{
    if(depth > maxDepth)
    {
        return Black.to_vec();
    }
    float minDistance = 1e16; //inf
    Renderable_Entity *chEntity;
    bool isIns = false;
    for(auto ptr : entitys)
    {
        vec3f ins = ptr->light_intersection(start, dir);
        if(ins.is_zero())
            continue;
        isIns = true;
        float dist = (ins - start).length();
        if(dist < minDistance)
        {
            minDistance = dist;
            chEntity = ptr;
        }
    }
    if(!isIns || minDistance >= 1e16)
    {
        return Black.to_vec();
    }
    vec3f ins = chEntity->light_intersection(start, dir);
    vec3f color = chEntity->render_point(ins, dir, entitys, lights);
    vec3f R = chEntity->get_reflex(ins, dir);
    vec3f T = chEntity->get_refraction(ins, dir);
	vec3f Ir = RayTracing(ins, R, depth + 1);
	vec3f It = RayTracing(ins, T, depth + 1);
	vec3f ret = color + Ir * ((Phong_Entity*)chEntity)->get_kd() + It * ((Phong_Entity*)chEntity)->get_ks();
	return ret;
}

vec3f RayTracing_BoundingBox(vec3f start, vec3f dir, int depth)
{
	if (depth > maxDepth)
	{
		return Black.to_vec();
	}
	float minDistance = 1e16; //inf
	Renderable_Entity *chEntity;
	bool isIns = false;
	for (auto Box : BoundingBoxes)
	{
		auto ptr = Box->get_lighting_entity(start, dir);
		if (ptr == nullptr)
		{
			continue;
		}
		vec3f ins = ptr->light_intersection(start, dir);
		if (ins.is_zero())
			continue;
		isIns = true;
		float dist = (ins - start).length();
		if (dist < minDistance)
		{
			minDistance = dist;
			chEntity = ptr;
		}
	}
	if (!isIns || minDistance >= 1e16)
	{
		return Black.to_vec();
	}
	vec3f ins = chEntity->light_intersection(start, dir);
	vec3f color = chEntity->render_point(ins, dir, entitys, lights);
	vec3f R = chEntity->get_reflex(ins, dir);
	vec3f T = chEntity->get_refraction(ins, dir);
	vec3f Ir = RayTracing_BoundingBox(ins, R, depth + 1);
	vec3f It = RayTracing_BoundingBox(ins, T, depth + 1);
	vec3f ret = color + Ir * ((Phong_Entity*)chEntity)->get_kd() + It * ((Phong_Entity*)chEntity)->get_ks();
	return ret;
}


void buildBoundingTree()
{
	vector<BoundingBox*> stdBox, specBox;
	printf("Building BoundingBox Tree...\n");
	for(auto entity : entitys)
	{
		vec3f bmin, bmax;
		entity->get_bounding(bmin, bmax);
		BoundingBox* box = new BoundingBox(bmin, bmax, entity);
		if(bmin == bmax)
		{
			specBox.push_back(box);
		}
		else
		{
			stdBox.push_back(box);
		}
	}
	while(stdBox.size() >= 2)
	{
		int lid = -1, rid = -1;
		double dist = double_inf;
		for (auto i = 0; i < stdBox.size(); ++i)
		{
			for (auto j = i + 1; j < stdBox.size(); ++j)
			{
				if (stdBox[i]->get_distance(stdBox[j]) < dist)
				{
					lid = i;
					rid = j;
					dist = stdBox[i]->get_distance(stdBox[j]);
				}
			}
		}
		BoundingBox* nboxptr = new BoundingBox(vector<BoundingBox*>{ stdBox[lid], stdBox[rid] });
		//delete stdBox[rid];
		//delete stdBox[lid];
		stdBox.erase(stdBox.begin() + rid);
		stdBox.erase(stdBox.begin() + lid);
		stdBox.push_back(nboxptr);
	}
	assert(stdBox.size() == 1);

	BoundingBoxes.clear();
	BoundingBoxes.push_back(stdBox[0]);
	for (auto i = 0; i < specBox.size(); ++i)
	{
		BoundingBoxes.push_back(specBox[i]);
	}
	printf("Building Finished!\n");
}
void draw(int LX, int LY, int RX, int RY, int Z, vec3f viewpoint, string framename = "RayTrace.bmp")
{
	int px = RX - LX + 1, py = RY - LY + 1;
	cleanframe();
	buildBoundingTree();
	for (int i = 0; i <= RY - LY; ++i)
	{
		int y = LY + i;
		printf("Rendering Line %d\n", i);
		for (int j = 0; j <= RX - LX; ++j)
		{
			int x = LX + j;
			int pos = (py - i - 1) * px + (px - j - 1);
			vec3f res = RayTracing_BoundingBox(viewpoint, vec3f(x, y, Z).norm(), 1);
			BufferR[pos] = resize(res.x);
			BufferG[pos] = resize(res.y);
			BufferB[pos] = resize(res.z);
		}
	}
	output(px, py, framename);
}
void createRandomScene(int ballNumber = 10)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	PointLight *light = new PointLight(vec3f(-1000, -1000, 0), White.to_norm());
	lights.push_back((Light*)light);

	std::uniform_int_distribution<int> xdis(-500, 500), ydis(-300, 0), zdis(800, 1300), rdis(30, 70), cdis(0, 2);
	for (int i = 0; i < ballNumber; ++i)
	{
		Color color;
		switch (cdis(gen))
		{
		case 0:
			color = DRed;
			break;
		case 1:
			color = DBlue;
			break;
		case 2:
			color = DGreen;
			break;
		default:
			color = DBlue;
			break;
		}
		int px = xdis(gen), py = ydis(gen), pz = zdis(gen);
		int pr = rdis(gen);
		auto *ball = new Sphere_Phong(vec3f(px, py, pz), pr, color);
		ball->set_ka(0.9); ball->set_kd(0.8); ball->set_ks(0.7); ball->set_n(40);
		entitys.push_back((Renderable_Entity*)ball);
	}
	Plane_Phong *plane = new Plane_Phong(vec3f(0, -1, 0), 120, DGray);
	plane->set_ka(0.9); plane->set_kd(0.8); plane->set_ks(0.8); plane->set_n(100);
	entitys.push_back((Renderable_Entity*)plane);
}
void createScene()
{
	Sphere_Phong *ball1 = new Sphere_Phong(vec3f(200, -50, 1000), 150, DRed);
	Sphere_Phong *ball2 = new Sphere_Phong(vec3f(-200, -50, 1000), 150, DBlue);
	Sphere_Phong *ball3 = new Sphere_Phong(vec3f(0, -350, 1500), 200, DGreen);
	ball1->set_ka(0.9); ball1->set_kd(0.8); ball1->set_ks(0.7); ball1->set_n(10);
	ball2->set_ka(0.9); ball2->set_kd(0.9); ball2->set_ks(0.6); ball2->set_n(40);
	ball3->set_ka(0.9); ball3->set_kd(0.9); ball3->set_ks(0.4); ball3->set_n(60);

	Plane_Phong *plane = new Plane_Phong(vec3f(0, -1, 0), 120, DGray);
	plane->set_ka(0.9); plane->set_kd(0.8); plane->set_ks(0.8); plane->set_n(100);

	entitys.push_back((Renderable_Entity*)ball1);
	entitys.push_back((Renderable_Entity*)ball2);
	entitys.push_back((Renderable_Entity*)ball3);
	entitys.push_back((Renderable_Entity*)plane);

	PointLight *light1 = new PointLight(vec3f(1000, 200, 0), White.to_norm());
	PointLight *light2 = new PointLight(vec3f(-1000, -1000, 0), White.to_norm());

	//lights.push_back((Light*)light1);
	lights.push_back((Light*)light2);
}
int main(int argc,char **argv)
{
	createScene();
	//createRandomScene();
	draw(-500, -500, 500, 500, 900, vec3f(0, -100, 0));
    return 0;
}
