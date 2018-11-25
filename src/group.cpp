#include "group.h"
#include "element.h"
#include "lens.h"
#include <algorithm>
#include <iostream>
using namespace std;

group::group(lens* parent):component(parent){
	//ctor
	width=0.75;
	range=0.25;
	position=0.8;
}

group::~group(){
	//dtor
}

controlPts group::getControls(){
	throw logic_error("Not Implemented: "+string(__func__));
}
void group::setControls(const controlPts& raw){
	throw logic_error("Not Implemented: "+string(__func__));
}

vector<ray> group::checkRays(const vector<ray>& rin){
	throw logic_error("Not Implemented: "+string(__func__));
}

rect group::getFullRect(const rect& parent){
	rect myrec=parent;
	myrec.w*=width;
	myrec.h*=front-back;//
	myrec.x+=(parent.w-myrec.w)/2;//target.center-myrec.w/2 = (target.x+target.w/2)-myrec.w/2 = target.x+(target.w-myrec.w)/2
	myrec.y+=parent.h*(1-front);
	return myrec;
}
rect group::getRect(const rect& parent){
	auto region=getFullRect(parent);
	region.y+=(1-position)*(region.h*range);
	region.h*=(1-range);
	return region;
}


void group::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		bgcol=0xff47da83,
		bordercol=0xff007a2d,
		rangecol=0xffe981be;
	auto myrec=getFullRect(target);
	//cout<<"drawing rect: ("<<myrec.x<<","<<myrec.y<<") "<<myrec.w<<"x"<<myrec.h<<endl;
	
	rect region=getRect(target);
	
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
