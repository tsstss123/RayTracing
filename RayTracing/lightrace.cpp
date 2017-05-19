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

const int BUFFERSIZE = 2000 * 2000;
const float minWeight = 0.001;

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
	return unsigned char(v);
}
void output(int px, int py)
{
	unsigned char head_buf[100] = { 66,77,54,8,7,0,0,0,0,0,54,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,1,0,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	FILE *fout = fopen("RayTrace.bmp", "wb");
	int *xlenp = (int *)&head_buf[18], *ylenp = (int *)&head_buf[22];
	*xlenp = px; *ylenp = py;
	fwrite(head_buf, 54, 1, fout);
	for (int i = 0; i < px * py; ++i) {
		fwrite(&BufferB[i], 1, 1, fout);
		fwrite(&BufferG[i], 1, 1, fout);
		fwrite(&BufferR[i], 1, 1, fout);
	}
	fclose(fout);
}

vector<Entity*> entitys;
vector<Light*> lights;

vec3f RayTracing(vec3f start, vec3f dir, float weight)
{
    if(weight < minWeight)
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
    if(!isIns)
    {
        return Black.to_vec();
    }
    vec3f ins = chEntity->light_intersection(start, dir);
    vec3f color = chEntity->render_point(ins, dir, entitys, lights);
    vec3f R = chEntity->get_reflex(ins, dir);
    vec3f T = chEntity->get_refraction(ins, dir);
	vec3f Ir = RayTracing(ins, R, weight * chEntity->get_weight_rx());
	vec3f It = RayTracing(ins, T, weight * chEntity->get_weight_rt());
	vec3f ret = color + Ir * ((Phong_Entity*)chEntity)->get_ks() + It * ((Phong_Entity*)chEntity)->get_ks();
	return ret;
}
void draw(int LX, int LY, int RX, int RY, int Z, vec3f viewpoint)
{
    int px = RX - LX + 1, py = RY - LY + 1;
	int pos = 0;
    for(int i = 0; i <= RX - LX; ++i)
    {
        int x = LX + i;
        for(int j = 0; j <= RY - LY; ++j)
        {
            int y = LY + j;
            //int pos = i * px + j;
            vec3f res = RayTracing(viewpoint, vec3f(x, y, Z).norm(), 1.0);
			if(!res.is_zero())
				printf("%d %d = (%f, %f, %f)\n", i, j, res.x, res.y, res.z);
            BufferR[pos] = resize(res.x);
            BufferG[pos] = resize(res.y);
            BufferB[pos] = resize(res.z);
			++pos;
        }
    }
	output(px, py);
}
/*
void lookit(int x, int y, int z){
    int offx = px / 2, offy = py / 2;
    for(int i = 0; i < sph.size(); ++i){
        Sphere &s = sph[i];
        float a = x * x + y * y + z * z;
        float cx = s.x, cy = s.y, cz = s.z;
        float b = 2 * x * (-cx) + 2 * y * (-cy) + 2 * z * (-cz);
        float c = cx * cx + cy * cy + cz * cz - s.r * s.r;
        float delta = b * b - 4 * a * c;

        if(delta <= 0)continue;
        float t = min((-b - sqrt(delta)) / (2 * a), (-b + sqrt(delta)) / (2 * a));
        assert(t >= 0);
        float tx = t * x, ty = t * y, tz = t * z;
        if(tz > Z[x + offx][y + offy])
            continue;
        Z[x + offx][y + offy] = tz;

        float nx = tx - s.x, ny = ty - s.y, nz = tz - s.z;
        float sqrtN = sqrt(nx * nx + ny * ny + nz * nz);
        nx /= sqrtN; ny /= sqrtN; nz /= sqrtN;
        float lx = lpx - tx, ly = lpy - ty, lz = lpz - tz;
        float sqrtL = sqrt(lx * lx + ly * ly + lz * lz);
        lx /= sqrtL; ly /= sqrtL; lz /= sqrtL;
        float I = s.p * (lx * nx + ly * ny + lz * nz);

        R[x + offx][y + offy] = max(0.f, min(255.f, I * s.c.r));
        G[x + offx][y + offy] = max(0.f, min(255.f, I * s.c.g));
        B[x + offx][y + offy] = max(0.f, min(255.f, I * s.c.b));

    }

}
*/
void createScene()
{
	Sphere_Phong *ball1 = new Sphere_Phong(vec3f(0, 0, 200), 30, Red);
	Sphere_Phong *ball2 = new Sphere_Phong(vec3f(100, 200, 500), 200, Blue);
	ball1->set_ka(0.9); ball1->set_kd(0.3); ball1->set_ks(0.6); ball1->set_n(2);
	ball2->set_ka(0.9); ball2->set_kd(0.2); ball2->set_ks(0.8); ball2->set_n(5);

	entitys.push_back((Entity*)ball1);
	entitys.push_back((Entity*)ball2);

	PointLight *light1 = new PointLight(vec3f(10, 20, 10));

	lights.push_back((Light*)light1);
}
int main(int argc,char **argv)
{
	createScene();
	draw(-400, -400, 399, 399, 400, vec3f(0, 0, 0));
    return 0;
}