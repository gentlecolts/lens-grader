#include "group.h"
#include "element.h"
#include "lens.h"
#include <algorithm>
#include <iostream>
using namespace std;

group::group(lens* parent):component(parent){
	//ctor
	width=0.5;
	range=0.75;
	position=0.8;
}

group::~group(){
	//dtor
}

controlPts group::getControls(){
	throw logic_error("Not Implemented");
}
void group::setControls(const controlPts& raw){
	throw logic_error("Not Implemented");
}
vector<std::shared_ptr<component>> group::getChildren(){
	throw logic_error("Not Implemented");
}

ray group::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

void group::drawTo(pbuffer &pixels,const rect &target){
	//throw logic_error("Not Implemented");
	const uint32_t
		bgcol=0xff47da83,
		bordercol=0xff007a2d,
		rangecol=0xffe981be;
	rect myrec=target;
	myrec.w*=width;
	myrec.h*=front-back;//
	myrec.x+=(target.w-myrec.w)/2;//target.center-myrec.w/2 = (target.x+target.w/2)-myrec.w/2 = target.x+(target.w-myrec.w)/2
	myrec.y+=target.h*(1-front);
	
	//cout<<"drawing rect: ("<<myrec.x<<","<<myrec.y<<") "<<myrec.w<<"x"<<myrec.h<<endl;
	
	rect region=myrec;
	region.h*=(1-range);
	region.y+=(1-position)*(myrec.h*range);
	
	const int
		x0=myrec.x,
		x1=myrec.x+myrec.w,
		y0=myrec.y,
		y1=myrec.y+myrec.h;
	
	for(int j=y0;j<y1;j++){
		for(int i=x0;i<x1;i++){
			auto col=bgcol;
			
			if(i==x0 || i==x1-1 || j==y0 || j==y1-1){
				col=bordercol;
			}else if(i>=region.x && i<=region.x+region.w && j>=region.y && j<=region.y+region.h){
				col=rangecol;
			}
			
			pixels.pixels[i+pixels.w*j]=col;
		}
	}
	
	for(auto groupcomponent:children){
		groupcomponent->drawTo(pixels,region);
	}
}

//TODO: use c++17 version
template<typename T>
constexpr const T& clamp(const T& v,const T& lo,const T& hi){
	return (v<lo)?lo:(hi<v)?hi:v;
}

void group::setFront(double f){
	front=clamp(f,0.0,1.0);
	reevaluateDims();
}
void group::setBack(double b){
	back=clamp(b,0.0,1.0);
	reevaluateDims();
}
void group::setWidth(double w){
	width=clamp(w,0.0,1.0);
}
void group::setRange(double r){
	range=clamp(r,0.0,1.0);
	reevaluateDims();
}
void group::setPosition(double p){
	position=clamp(p,0.0,1.0);
}

double group::getFront(){
	return front;
}
double group::getBack(){
	return back;
}
double group::getWidth(){
	return width;
}
double group::getRange(){
	return range;
}
double group::getPosition(){
	return position;
}

void group::reevaluateDims(){
}
