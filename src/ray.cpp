#include "ray.h"
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace std;

point::point():x(0),y(0){}
point::point(double x0,double y0):x(x0),y(y0){}

ray::ray():p(),dir(){}
ray::ray(const point& origin,const point& direction):p(origin),dir(direction){}
ray::ray(double x,double y,double dx,double dy):p(x,y),dir(dx,dy){}


ray& ray::fromPoints(const point& p0,const point& p1){
	return fromPoints(p0.x,p0.y,p1.x,p1.y);
}
ray& ray::fromPoints(double x0,double y0,double x1,double y1){
	p.x=x0;
	p.y=y0;

	dir.x=x1-x0;
	dir.y=y1-y0;
	
	return *this;
}
void ray::normalize(){
	const auto d=sqrt(dir.x*dir.x+dir.y*dir.y);
	dir.x/=d;
	dir.y/=d;
}
ray ray::normalized() const{
	ray r=*this;
	r.normalize();
	return r;
}


ray ray::refract(const ray& normal,double ior){
	if(lost){
		return *this;
	}
	
	const auto dot=[](const point& v0,const point& v1){
		return v0.x*v1.x+v0.y*v1.y;
	};
	
	auto n=normal.normalized().dir;
	auto l=normalized().dir;
	auto ctheta1=-dot(n,l);
	
	//*
	if(ctheta1<0){
		//n=point(1+2*ctheta1*n.x,1+2*ctheta1*n.y);
		//l=point(1+2*ctheta1*l.x,1+2*ctheta1*l.y);
		n=point(-n.x,-n.y);
		ctheta1=-dot(n,l);
	}
	//*/
	
	//TODO: it seems like this will return false if the asin is nan, which is good, is this reliable?
	bool isCritical=acos(ctheta1)>asin(ior/currentIOR);
	auto r=currentIOR/ior,a=isCritical?0:r*ctheta1-sqrt(1-r*r*(1-ctheta1*ctheta1));
	
	if(!isfinite(a)){
		printf("it's a...%f vs %f - critical: %d\n",r*r,1-ctheta1*ctheta1,isCritical);
	}
	point vrefract(r*l.x+a*n.x,r*l.y+a*n.y);
	
	ray rout(normal.p,vrefract);
	rout.currentIOR=ior;
	rout.lost=isCritical;
	return rout;
}


void rayPath::refract(const ray& normal,double ior){
	auto newray=segments.back().refract(normal,ior);
	segments.push_back(newray);
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
		//printf("drawing pixel at %i %i\n",x,y);
		if(x>=0 && x<w && y>=0 && y<h){
			pixels[x+w*y]=color;
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
