#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

#include <cinttypes>
#include <vector>

//TODO: use std::valarray instead
struct point{
	double x,y;
	point();
	point(double x0,double y0);
};

struct ray{
	point p,dir;
	
	double intensity=1,currentIOR=1;
	//double wavelength;//TODO
	
	ray();
	ray(const point& origin,const point& direction);
	ray(double x,double y,double dx,double dy);

	void fromPoints(const point& p0,const point& p1);
	void fromPoints(double x0,double y0,double x1,double y1);
	
	//TODO: add material parameter (and make material struct)
	//TODO: return both a reflect and a refract ray?  will reflection be needed?
	ray refract(const ray& normal);
};

struct rayPath{
	std::vector<ray> segments;
	void refract(const ray& normal);
};

struct pbuffer{
	uint32_t *pixels;
	int w,h;
	pbuffer();
	pbuffer(uint32_t *buf,int width,int height);
	
	//assuming points are in pixel coordinates, origin at top-left
	void drawLinePixels(const double x0,const double y0,const double x1,const double y1,uint32_t color=0x000000);
	void drawLinePixels(const point& p0,const point& p1,uint32_t color=0x000000);
	//assuming points are 0.0-1.0, origin at bottom-left
	void drawLineNormalized(const double x0,const double y0,const double x1,const double y1,uint32_t color=0x000000);
	void drawLineNormalized(const point& p0,const point& p1,uint32_t color=0x000000);
};

struct rect{
	double x,y,w,h;
	rect();
	rect(double x0,double y0,double width,double height);
};

#endif // RAY_H_INCLUDED
