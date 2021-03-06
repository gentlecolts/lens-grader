/*Implements code found in group.h
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
#include "group.h"
#include "element.h"
#include "lens.h"
#include <algorithm>
#include <iostream>
using namespace std;

void group::initializeControlVars(){
	setControlVars(front,back,width,range,movementMultiplier);
}
shared_ptr<component> group::copy() const{
	auto gcopy=make_shared<group>(*this);
	gcopy->initializeControlVars();
	return gcopy;
}

group::group(lens* parent):component(parent){
	//ctor
	//width=0.75;
	range=0.25;
	position=0.8;
	
	initializeControlVars();
}

group::~group(){
	//dtor
}

vector<double> group::getControls(){
	//group's controls
	auto controls=copyControlVars();
	
	//printf("Group added %i control vars\n",controls.size());
	
	//individual element controls
	appendChildrenControlsTo(controls);
	
	//and return
	return controls;
}
vector<controlRef> group::getControlRefs(){
	//we need a copy, we dont want to modify controlVars
	vector<controlRef> controls(controlVars.begin(),controlVars.end());
	
	//individual element controls
	appendChildrenControlRefsTo(controls);
	
	//and return
	return controls;
}
void group::setControls(vector<double>& controls){
	consumeControlVars(controls);
	for(auto& child:children){
		child->setControls(controls);
	}
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
	myrec.h*=max(front,back)-min(front,back);//
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

groupGeometry group::getGeometry(){
	//save the state of the position, this function returns the geometry when at position 0, and the y translation needed to reach position=1
	//NOTE: getSurface will eventually call this object's getRealSize, so it does take position into account
	auto originalPos=position;
	position=0;
	
	groupGeometry geo;
	for(auto child:children){
		if(auto e=dynamic_pointer_cast<element>(child)){
			geo.lenses.push_back(e->getSurface());
		}
	}
	
	//now get the front and back.  this can probably be simplified, but it works
	const auto y0=getRealSize().y;
	position=1;
	auto y1=getRealSize().y;
	geo.travel=y1-y0;
	
	//reset the position
	position=originalPos;
	
	return geo;
}
