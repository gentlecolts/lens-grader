#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

#include <cinttypes>

//TODO: use std::valarr instead
struct point{
	double x,y;
	point();
	point(double x0,double y0);
};

struct ray{
	point p,dir;
	ray();
	ray(const point& origin,const point& direction);
	ray(double x,double y,double dx,double dy);

	void fromPoints(const point& p0,const point& p1);
	void fromPoints(double x0,double y0,double x1,double y1);
};

struct pbuffer{
	uint32_t *pixels;
	int w,h;
	pbuffer();
	pbuffer(uint32_t *buf,int width,int height);
};

struct rect{
	double x,y,w,h;
	rect();
	rect(double x0,double y0,double width,double height);
};

#endif // RAY_H_INCLUDED
