/**
  ******************************************************************************
  * @file           : Math2D.h
  * @brief          : vector math implementation
  * @author			: Craig Beaubien
  * 				: CPE 187
  ******************************************************************************
  * inspired by Game Physics Cookbook by Gabor Szauer
  ******************************************************************************
*/
#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define FALSE			0
#define TRUE			1

typedef struct
{
	int x, y;
} vec2i;

int Doti(const vec2i* l, const vec2i* r);
float Magnitudei(const vec2i* v);
int MagnitudeSqi(const vec2i* v);
float Distancei(const vec2i* p1, const vec2i* p2);
vec2i Addi(const vec2i *A, const vec2i *B);
vec2i Subtracti(const vec2i *A, const vec2i *B);
vec2i Multiplyi(const vec2i *A, const float scalar);
void Normalizei(vec2i *v);
vec2i Normalizedi(const vec2i *v);
float Anglei(const vec2i *A, const vec2i *B);

/*-----------------------------------------------------------------------*/

typedef struct
{
	float x, y;
} vec2f;
vec2f make_vec2f(float x, float y);
float Dotf(const vec2f* l, const vec2f* r);
float Magnitudef(const vec2f* v);
float MagnitudeSqf(const vec2f* v);
void Normalizef(vec2f *v);
float Distancef(const vec2f* p1, const vec2f* p2);
vec2f Normalizedf(const vec2f *v);
float Anglef(const vec2f *A, const vec2f *B);
vec2f Addf(const vec2f *a, const vec2f *b);
vec2f Subtractf(const vec2f *A, const vec2f *B);
vec2f Multiplyf(const vec2f *A, float scalar);
int IsEqual(const vec2f *A, const vec2f *B);
