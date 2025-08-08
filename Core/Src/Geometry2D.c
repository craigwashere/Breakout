/**
  ******************************************************************************
  * @file           : Geometry2D.c
  * @brief          : geometry implementation
  * @author			: Craig Beaubien
  * 				: CPE 187
  ******************************************************************************
  * inspired by Game Physics Cookbook by Gabor Szauer
  ******************************************************************************
*/

#include "Geometry2D.h"

vec2f GetMin(const Rectangle2Df* rect)
{
	vec2f p1 = rect->origin;
	vec2f p2 = Addf(&rect->origin, &rect->size);

	vec2f r;
	r.x = fminf(p1.x, p2.x);
	r.y = fminf(p1.y, p2.y);
	return r;
}

vec2f GetMax(const Rectangle2Df* rect)
{
	vec2f p1 = rect->origin;
	vec2f p2 = Addf(&rect->origin, &rect->size);

	vec2f r;
	r.x = fmaxf(p1.x, p2.x);
	r.y = fmaxf(p1.y, p2.y);
	return r;
}

float Length(const Line2Df *line)
{
	vec2f r = Subtractf(&line->end, &line->start);
	return Magnitudef(&r);
}

float LengthSq(const Line2Df *line)
{
	vec2f r = Subtractf(&line->end, &line->start);
	return MagnitudeSqf(&r);
}

uint8_t PointOnLine(const Point2Df * p, const Line2Df * line)
{
	// Find the slope
	float dy = (line->end.y - line->start.y);
	float dx = (line->end.x - line->start.x);
	float M = dy / dx;

	// Find the Y-Intercept
	float B = line->start.y - M * line->start.x;

	// Check line equation
	return CMP(p->y, M * p->x + B);
}

uint8_t PointInCircle(const Point2Df *point, const Circlef *c)
{
	Line2Df line;
	line.start.x = point->x;
	line.start.y = point->y;

	line.end.x = c->position.x;
	line.end.y = c->position.y;

	if (LengthSq(&line) < c->radius * c->radius)
		return TRUE;

	return FALSE;
}

uint8_t PointInRectangle(const Point2Df *point, const Rectangle2Df *rectangle)
{
	vec2f min = GetMin(rectangle);
	vec2f max = GetMax(rectangle);

	return min.x   <= point->x
		&& min.y   <= point->y
		&& point->x <= max.x
		&& point->y <= max.y;
}

uint8_t CircleRectangle(const Circlef * circle, const Rectangle2Df * rect)
{
	vec2f min = GetMin(rect);
	vec2f max = GetMax(rect);

	Point2Df closest_point = circle->position;
	if (closest_point.x < min.x)			closest_point.x = min.x;
	else if (closest_point.x > max.x) 		closest_point.x = max.x;

	closest_point.y = (closest_point.y < min.y) ? min.y : closest_point.y;
	closest_point.y = (closest_point.y > max.y) ? max.y : closest_point.y;

	Line2Df line;
	line.start.x = circle->position.x;
	line.start.y = circle->position.y;
	line.end.x =  closest_point.x;
	line.end.y = closest_point.y;

	return LengthSq(&line) <= circle->radius * circle->radius;
}
