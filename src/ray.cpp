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


pbuffer::pbuffer():pixels(0),w(0),h(0){}
pbuffer::pbuffer(uint32_t *buf,int width,int height):pixels(buf),w(width),h(height){}

rect::rect():x(0),y(0),w(0),h(0){}
rect::rect(double x0,double y0,double width,double height):x(x0),y(y0),w(width),h(height){}
