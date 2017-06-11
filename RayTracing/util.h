#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <cassert>
using std::sqrt;

const double eps = 1e-4;
const double double_inf = 1e16;

struct vec3f
{
    float x, y, z;
    vec3f():x(0),y(0),z(0){}
    vec3f(float _x, float _y, float _z):x(_x),y(_y),z(_z){}
    bool operator == (const vec3f &rhs)const{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool operator != (const vec3f &rhs)const{
		return !(*this == rhs);
	}
    float operator * (const vec3f &rhs)const{
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    vec3f operator * (const float b)const{
        return vec3f(b * x, b * y, b * z);
    }
    vec3f operator + (const vec3f &rhs)const{
        return vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    vec3f operator + (const int rhs)const{
		return vec3f(x + rhs, y + rhs, z + rhs);
	}
    vec3f operator - (const vec3f &rhs)const{
        return vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    vec3f operator - (const int rhs)const{
		return vec3f(x - rhs, y - rhs, z - rhs);
	}
    float length(){
        return sqrt(x * x + y * y + z * z);
    }
    vec3f norm(){
        float len = this->length();
        return vec3f(x / len, y / len, z / len);
    }
    bool is_zero(){
        return x < eps && y < eps && z < eps;
    }
};

struct Color
{
    unsigned char r, g, b, a;
    Color():r(0),g(0),b(0),a(0){}
    Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 0):
        r(_r),g(_g),b(_b),a(_a){}
	vec3f to_vec()const{
		return vec3f(r, g, b);
	}
	vec3f to_norm()const{
		return vec3f(r / 255.f, g / 255.f, b / 255.f);
	}
};
const Color Black(0, 0, 0), Red(255, 0, 0), Green(0, 255, 0), Blue(0, 0, 255), White(255, 255, 255);
const Color DRed(180, 50, 50), DBlue(50, 50, 180), DGreen(50, 180, 50), DGray(200, 200, 200);

#endif
