/*Declares ray and ray-related structs
Copyright (C) 2018  Jack Phoebus

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

#include <cinttypes>
#include <vector>
#include <ostream>

//TODO: use std::valarray instead
struct point{
	double x,y;
	point();
	point(double x0,double y0);
	friend std::ostream& operator<<(std::ostream& strm, const point& p);
};

struct ray{
	point p,dir;
	
	double intensity=1,currentIOR=1;
	//double wavelength;//TODO
	
	bool lost=false;
	
	ray();
	ray(const point& origin,const point& direction);
	ray(double x,double y,double dx,double dy);

	ray& fromPoints(const point& p0,const point& p1);
	ray& fromPoints(double x0,double y0,double x1,double y1);
	
	void normalize();
	ray normalized() const;
	
	//TODO: add material parameter (and make material struct?) or is ior sufficient?
	//TODO: return both a reflect and a refract ray?  will reflection be needed?
	ray refract(const ray& normal,double ior);
};

struct rayPath{
	std::vector<ray> segments;
	point target;
	bool shouldBeLost=false;
	void refract(const ray& normal,double ior);
};

struct pbuffer{
	uint32_t *pixels;
	int w,h;
	pbuffer();
	pbuffer(uint32_t *buf,int width,int height);
	
	//assuming points are in pixel coordinates, origin at top-left
	void drawLinePixels(const double x0,const double y0,const double x1,const double y1,uint32_t color=0xff000000);
	void drawLinePixels(const point& p0,const point& p1,uint32_t color=0xff000000);
	//assuming points are 0.0-1.0, origin at bottom-left
	void drawLineNormalized(const double x0,const double y0,const double x1,const double y1,uint32_t color=0xff000000);
	void drawLineNormalized(const point& p0,const point& p1,uint32_t color=0xff000000);
};

struct rect{
	double x,y,w,h;
	rect();
	rect(double x0,double y0,double width,double height);
};

//TODO: this should actually flip the rect within a pixel buffer, then remap the point within that flipped rect
template<typename T>
T flipInsideBuffer(const pbuffer& pix,const rect& r, const T& y){
	/*
	//where is y relative to the rect's y
	const T dy=y-r.y;
	
	//where is the rect's bottom y with respect to the buffer
	const T rectY=pix.h-1-r.y;
	
	//up relative to the rect is down relative to the pixel buffer
	return rectY-dy;
	/*/
	return pix.h-1-y;
	//*/
}

#endif // RAY_H_INCLUDED
