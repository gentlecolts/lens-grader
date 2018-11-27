#include "ray.h"
#include <cmath>
#include <algorithm>
using namespace std;

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

ray ray::refract(const ray& normal){
	//TODO
	return ray(normal.p,dir);
}


void rayPath::refract(const ray& normal){
}


pbuffer::pbuffer():pixels(0),w(0),h(0){}
pbuffer::pbuffer(uint32_t *buf,int width,int height):pixels(buf),w(width),h(height){}

void pbuffer::drawLinePixels(const point& p0, const point& p1,uint32_t color){
	drawLinePixels(p0.x,p0.y,p1.x,p1.y,color);
}
void pbuffer::drawLinePixels(const double x0, const double y0, const double x1, const double y1,uint32_t color){
	const int dx=x1-x0,dy=y1-y0,n=max(abs(dx),abs(dy));
	for(int i=0;i<=n;i++){
		//TODO: do proper clipping and account for n=0
		const int x=x0+(i*dx)/n,y=y0+(i*dy)/n;
		if(x>=0 && x<w && y>=0 && y<h){
			pixels[x+w*h]=color;
		}
	}
}
void pbuffer::drawLineNormalized(const point& p0, const point& p1, uint32_t color){
	drawLineNormalized(p0.x,p0.y,p1.x,p1.y,color);
}
void pbuffer::drawLineNormalized(const double x0, const double y0, const double x1, const double y1, uint32_t color){
	drawLinePixels(x0*(w-1),(1-y0)*(h-1),x1*(w-1),(1-y1)*(h-1),color);
}



rect::rect():x(0),y(0),w(0),h(0){}
rect::rect(double x0,double y0,double width,double height):x(x0),y(y0),w(width),h(height){}