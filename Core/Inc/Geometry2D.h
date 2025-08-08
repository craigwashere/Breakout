/**
  ******************************************************************************
  * @file           : Geometry2D.h
  * @brief          : geometry implementation
  * @author			: Craig Beaubien
  * 				: CPE 187
  ******************************************************************************
  * inspired by Game Physics Cookbook by Gabor Szauer
  ******************************************************************************
*/

#include "math2D.h"
#include <float.h>
#include <stdint.h>

#define CMP(x, y) \
		(fabsf((x)-(y)) <= FLT_EPSILON * \
				fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

#define CLAMP(number, minimum, maximum) \
		number = (number < minimum) ? minimum : ( \
				(number > maximum) ? maximum : number \
		)

typedef vec2f Point2Df;
typedef vec2i Point2Di;

typedef struct Circlef
{
	Point2Df position;
	float radius;
} Circlef;

typedef struct Circlei
{
	Point2Di position;
	float radius;
} Circlei;

typedef struct Line2Df
{
	Point2Df start, end;
} Line2Df;

float Length(const Line2Df *line);
float LengthSq(const Line2Df *line);

typedef struct Rectangle2Df
{
	Point2Df origin;
	vec2f size;
} Rectangle2Df;

vec2f GetMin(const Rectangle2Df* rect);
vec2f GetMax(const Rectangle2Df* rect);

uint8_t PointOnLine(const Point2Df * point, const Line2Df * line);
uint8_t PointInCircle(const Point2Df * point, const Circlef * c);
uint8_t PointInRectangle(const Point2Df * point, const Rectangle2Df * rectangle);
