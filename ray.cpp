#include "ray.h"

point::point():x(0),y(0){}
point::point(double x0,double y0):x(x0),y(y0){}

ray::ray():p(),dir(){}
ray::ray(const point& origin,const point& direction):p(origin),dir(direction){}
ray::ray(double x,double y,double dx,double dy):p(x,y),dir(dx,dy){}

void ray::fromPoints(const point& p0,const point& p1){
	fromPoints(p0.x,p0.y,p1.x,p1.y);
}
void ray::fromPoints(double x0,double y0,double x1,double y1){
	p.x=x0;
	p.y=y0;

	dir.x=x1-x0;
	dir.y=y1-y0;
}
