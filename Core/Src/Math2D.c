/**
  ******************************************************************************
  * @file           : Math2D.c
  * @brief          : vector math implementation
  * @author			: Craig Beaubien
  * 				: CPE 187
  ******************************************************************************
  * inspired by Game Physics Cookbook by Gabor Szauer
  ******************************************************************************
*/
#include "Math2D.h"

int Doti(const vec2i* l, const vec2i* r)
{
    return (l->x*r->x) + (l->y*r->y);
}

float Magnitudei(const vec2i* v)
{
    return sqrt(Doti(v, v));
}

int MagnitudeSqi(const vec2i* v)
{
	return Doti(v, v);
}

float Distancei(const vec2i* p1, const vec2i* p2)
{
	vec2i t = Subtracti(p1, p2);
	return Magnitudei(&t);
}

vec2i Addi(const vec2i *A, const vec2i *B)
{
	vec2i r;
	r.x = A->x + B->x;
	r.y = A->x + B->y;
	return r;
}

vec2i Subtracti(const vec2i *A, const vec2i *B)
{
	vec2i r;
	r.x = A->x - B->x;
	r.y = A->x - B->y;
	return r;
}

vec2i Multiplyi(const vec2i *A, const float scalar)
{
	vec2i r;
	r.x = MAX(1, (A->x*scalar + 0.5));
	r.y = MAX(1, (A->y*scalar + 0.5));
	return r;
}

void Normalizei(vec2i *v)
{
	vec2i r = Multiplyi(v, (1.0/Magnitudei(v)));
	v->x = r.x;
	v->y = r.y;
}

vec2i Normalizedi(const vec2i *v)
{
	return Multiplyi(v, (1.0/Magnitudei(v)));
}

float Anglei(const vec2i *A, const vec2i *B)
{
	float m = sqrt(MagnitudeSqi(A) * MagnitudeSqi(B));
	return acos(Doti(A, B) / m);
}

/*-----------------------------------------------------------------------*/
vec2f make_vec2f(float x, float y)
{
	vec2f r;
	r.x = x;
	r.y = y;
	return r;
}

float Dotf(const vec2f* l, const vec2f* r)
{     return (l->x*r->x) + (l->y*r->y);	}

float Magnitudef(const vec2f* v)
{    return sqrt(Dotf(v, v));			}

float MagnitudeSqf(const vec2f* v)
{	return Dotf(v, v);					}

void Normalizef(vec2f *v)
{
	vec2f r = Multiplyf(v, (1.0/Magnitudef(v)));
	v->x = r.x;
	v->y = r.y;
}

float Distancef(const vec2f* p1, const vec2f* p2)
{
	vec2f t = Subtractf(p1, p2);
	return Magnitudef(&t);
}
vec2f Normalizedf(const vec2f *v)
{
	return Multiplyf(v, (1.0/Magnitudef(v)));
}

float Anglef(const vec2f *A, const vec2f *B)
{
	float m = sqrt(MagnitudeSqf(A) * MagnitudeSqf(B));
	return acos(Dotf(A, B) / m);
}

vec2f Addf(const vec2f *A, const vec2f *B)
{
	return make_vec2f(A->x + B->x, A->y + B->y);
}

vec2f Subtractf(const vec2f *A, const vec2f *B)
{
	return make_vec2f(A->x - B->x, A->y - B->y);
}

vec2f Multiplyf(const vec2f *A, float scalar)
{
	return make_vec2f(A->x*scalar, A->y*scalar);
}

int IsEqual(const vec2f *A, const vec2f *B)
{
	return (A->x == B->x) && (A->y == B->y);
}

vec2f clamp(vec2f input, vec2f min, vec2f max)
{
	return make_vec2f(
			fmaxf(min.x, fminf(input.x, max.x))
		  , fmaxf(min.y, fminf(input.y, max.y)));
}
