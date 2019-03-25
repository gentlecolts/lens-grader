#include "group.h"
#include "element.h"
#include "lens.h"
#include <algorithm>
#include <iostream>
using namespace std;

group::group(lens* parent):component(parent){
	//ctor
	//width=0.75;
	range=0.25;
	position=0.8;
	
	setControlVars(front,back,width,range,movementMultiplier);
}

group::~group(){
	//dtor
}

vector<double> group::getControls(){
	//group's controls
	auto controls=copyControlVars();
	
	printf("Group added %i control vars\n",controls.size());
	
	//individual element controls
	addChildrenControllsTo(controls);
	
	//and return
	return controls;
}
void group::setControls(vector<double>& controls){
	throw logic_error("Not Implemented: "+string(__func__));
}

void group::bounceRays(vector< rayPath >& paths){
	//TODO: elements can overlap, this needs to be handled, this is just a simplification
	//TODO: alternatively, assert that a ray cannot enter a new element until it has exited another.  this is what is currently below, but it's an assertion of how the geometry behaves when overlapped
	for(auto l:children){
		l->bounceRays(paths);
	}
}


rect group::getFullRect(const rect& parent) const{
	rect myrec=parent;
	myrec.w*=width;
	myrec.h*=front-back;//
	myrec.x+=(parent.w-myrec.w)/2;//target.center-myrec.w/2 = (target.x+target.w/2)-myrec.w/2 = target.x+(target.w-myrec.w)/2
	myrec.y+=parent.h*back;
	return myrec;
}
rect group::getRect(const rect& parent) const{
	auto region=getFullRect(parent);
	auto truePosition=0.5+(position-0.5)*(2*movementMultiplier-1);
	region.y+=truePosition*region.h*range;
	region.h*=1-range;
	return region;
}
rect group::getRealSize() const{
	const auto r=getRect(parent->getRealSize());
	//printf("Group is bounded by: (%f, %f), (%f, %f)\n",r.x,r.y,r.w,r.h);
	return r;
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
			//X is fine, but y needs to flip
			const int x=i,y=flipInsideBuffer(pixels,target,j);
			
			auto col=bgcol;
			if(i==x0 || i==x1-1 || j==y0 || j==y1-1){
				col=bordercol;
			}else if(i>=region.x && i<=region.x+region.w && j>=region.y && j<=region.y+region.h){
				col=rangecol;
			}
			
			//TODO: check against buffer boundaries
			pixels.pixels[x+pixels.w*y]=col;
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
void group::setMovementMultiplier(double m){
	movementMultiplier=clamp(m,0.0,1.0);
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
double group::getMovementMultiplier(){
	return movementMultiplier;
}


void group::reevaluateDims(){
	//TODO: the group has just been modified, if it's now overlapping any other groups, we need to adjust those groups appropriately
}
