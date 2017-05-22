//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <memory>

#include "util.h"
#include "light.h"
#include "entity.h"

using std::vector;
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
void output(int px, int py, string fname = "RayTrace.bmp")
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

vector<Entity*> entitys;
vector<Light*> lights;

vec3f RayTracing(vec3f start, vec3f dir, int depth)
{
    if(depth > maxDepth)
    {
        return Black.to_vec();
    }
    float minDistance = 1e16; //inf
    Entity *chEntity;
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
void draw(int LX, int LY, int RX, int RY, int Z, vec3f viewpoint, string framename)
{
    int px = RX - LX + 1, py = RY - LY + 1;
	cleanframe();
    for(int i = 0; i <= RY - LY; ++i)
    {
        int y = LY + i;
        for(int j = 0; j <= RX - LX; ++j)
        {
            int x = LX + j;
			if (i == 584 && j == 66)
			{
				int a = 0;
				++a;
			}
            int pos = (py - i - 1) * px + (px - j - 1);
            vec3f res = RayTracing(viewpoint, vec3f(x, y, Z).norm(), 1);
			//if(!res.is_zero())
				//printf("%d %d = (%f, %f, %f)\n", i, j, res.x, res.y, res.z);
            BufferR[pos] = resize(res.x);
            BufferG[pos] = resize(res.y);
            BufferB[pos] = resize(res.z);
        }
    }
	output(px, py, framename);
}

void createScene()
{
	Sphere_Phong *ball1 = new Sphere_Phong(vec3f(200, -50, 1000), 150, DRed);
	Sphere_Phong *ball2 = new Sphere_Phong(vec3f(-200, -50, 1000), 150, DBlue);
	Sphere_Phong *ball3 = new Sphere_Phong(vec3f(0, -350, 1500), 200, DGreen);
	ball1->set_ka(0.9); ball1->set_kd(0.8); ball1->set_ks(0.7); ball1->set_n(10);
	ball2->set_ka(0.9); ball2->set_kd(0.9); ball2->set_ks(0.6); ball2->set_n(50);
	ball3->set_ka(0.9); ball3->set_kd(0.9); ball3->set_ks(0.3); ball3->set_n(50);

	Plane_Phong *plane = new Plane_Phong(vec3f(0, -1, 0), 120, DGray);
	plane->set_ka(0.9); plane->set_kd(0.8); plane->set_ks(0.8); 
	plane->set_n(100);

	entitys.push_back((Entity*)ball1);
	entitys.push_back((Entity*)ball2);
	entitys.push_back((Entity*)ball3);
	entitys.push_back((Entity*)plane);

	PointLight *light1 = new PointLight(vec3f(1000, 200, 0));
	PointLight *light2 = new PointLight(vec3f(-1000, -1000, 0));

	//lights.push_back((Light*)light1);
	lights.push_back((Light*)light2);
}
int main(int argc,char **argv)
{
	createScene();
	for (int i = 400; i < 600; i += 4) {
		char buf[10];
		sprintf(buf, "%03d.bmp", i);
		draw(-400, -400, 400, 400, 900, vec3f(0, -i, 0), string(buf));
		printf("%d / 400\n", i);
	}
    return 0;
}
