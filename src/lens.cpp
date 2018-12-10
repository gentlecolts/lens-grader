#include "lens.h"
#include "group.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;


lens::lens(int groupCount, double focalLen):component(nullptr),focalLength(focalLen){
	groupCount=max(groupCount,1);//must have at least one group
	
	for(int i=0;i<groupCount;i++){
		auto g=newChildComponent<group>(this);
		g->setFront((double(i+1))/groupCount);
		g->setBack((double(i))/groupCount);
	}
	cout<<"made a lens with "<<children.size()<<" children"<<endl;
}


lens::lens(std::vector<double> groupDivs,double focalLen):lens(groupDivs.size()+1,focalLen){
	//sort the dividers
	sort(groupDivs.begin(),groupDivs.end());
	
	//set each in order
	for(int i=0;i<children.size();i++){
		const double
			back=i?groupDivs[i-1]:0,
			front=(i==groupDivs.size())?1:groupDivs[i];
		auto g=dynamic_pointer_cast<group>(children[i]);
		g->setBack(back);
		g->setFront(front);
		cout<<"group from "<<back<<" to "<<front<<endl;
	}
}

lens::~lens(){
	//dtor
}

vector< rayPath > lens::initializeRays(double distanceFromFront, int initialCount){
	//The rays produced by this funciton are in real world coordinates, where (0,0) is the center of the image sensor
	
	if(isfinite(distanceFromFront)){
	}else{
	}
}

controlPts lens::getControls(){
	throw logic_error("Not Implemented: "+string(__func__));
}
void lens::setControls(const controlPts& raw){
	throw logic_error("Not Implemented: "+string(__func__));
}

void lens::bounceRays(vector< rayPath >& paths){
	throw logic_error("Not Implemented: "+string(__func__));
}

rect lens::getRect(const rect& parent) const{
	//determine the actual shape of the bounding box
	const double
	cx=parent.x+parent.w/2,
	cy=parent.y+parent.h/2,
	ratio=(physicalLength+sensorToBack)/(physicalLength/aperature);//total length / width
	double h=parent.h,w=h/ratio;
	
	//cout<<w<<" "<<h<<endl;
	
	if(w>parent.w){
		h=h*parent.w/w;
		w=parent.w;
	}
	
	//create new bounds with the correct dimensions
	rect lensRec;
	lensRec.w=w;
	lensRec.h=h*physicalLength/(physicalLength+sensorToBack);
	lensRec.x=cx-lensRec.w/2;
	lensRec.y=
		cy-(parent.h-lensRec.h)/2//center of rectangle
		-lensRec.h/2;//top of rec
	return lensRec;
}
rect lens::getRealSize() const{
	const auto width=physicalLength/aperature;
	return rect(-width/2,0,width,physicalLength);
}


void lens::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		lenscolor=0xffdacd47,
		sensorcolor=0xff5caee5;
	
	auto lensRec=getRect(target);
	//cout<<lensRec.w<<" "<<lensRec.h<<" "<<lensRec.x<<" "<<lensRec.y<<endl;
	
	//TODO: clip target rect to buffer's size before this loop, in case of bad input
	
	//draw lens area
	for(int j=lensRec.y;j<(int)(lensRec.y+lensRec.h);j++){
		for(int i=lensRec.x;i<(int)(lensRec.x+lensRec.w);i++){
			pixels.pixels[i+pixels.w*j]=lenscolor;
		}
	}
	
	//draw sensor area
	//this is a bit more interesting, a trapezoid with the top being the mount radius and the bottom being the image circle
	
	const int j0=lensRec.y+lensRec.h,j1=target.y+target.h;
	const double realtopixel=lensRec.w/(physicalLength/aperature);//pixel width of the lens divided by "real" width of the lens, as a conversion ratio point
	const double cx=target.x+target.w/2;
	
	for(int j=j0;j<j1;j++){
		//how wide should we be
		const double t=((double)(j-j0))/(j1-1-j0);
		const int realwidth=realtopixel*(mountRadius+t*(imageCircleRadius-mountRadius));//note this is actually half of the width, but that's what we want anyways
		
		for(int i=cx-realwidth;i<=cx+realwidth;i++){
			pixels.pixels[i+pixels.w*j]=sensorcolor;
		}
	}
	
	//draw each group
	//cout<<"drawing groups"<<endl;
	for(auto groupcomponent:children){
		groupcomponent->drawTo(pixels,lensRec);
	}
	
	//display rays
	auto rays=initializeRays(numeric_limits<double>::infinity(),10);
	//bounceRays(rays);
	
	//draw just the initial segment
	const auto halfwidth=physicalLength/(2*aperature);
	const auto pointRemap=[&](const point& p){
		//first convert the input point from real-world coordinates into into the coordinate space such that x=[-1,1], y=[0,1]
		//then convert it to the display surface space
		return point(lensRec.x+((p.x/halfwidth)+1)*lensRec.w/2,lensRec.y+(p.y/physicalLength)*lensRec.h);
	};
	for(auto& ray:rays){
		pixels.drawLinePixels(pointRemap(ray.segments[0].p),pointRemap(ray.target));
	}
}

void lens::setPosition(double pos){
	for(auto child:children){
		if(auto g=dynamic_pointer_cast<group>(child)){
			g->setPosition(pos);
		}
	}
}

shared_ptr<group> lens::getBackGroup(){
	//children[0] is back
	return dynamic_pointer_cast<group>(children.front());
}
shared_ptr<group> lens::getFrontGroup(){
	return dynamic_pointer_cast<group>(children.back());
}
const vector<shared_ptr<group>> lens::getGroups(){
	vector<shared_ptr<group>> groups(children.size());
	std::transform(children.begin(),children.end(),groups.begin(),[](auto ptr){
		return dynamic_pointer_cast<group>(ptr);
	});
	
	return groups;
}

double lens::getScore(){
	auto rays=initializeRays(numeric_limits<double>::infinity(),10);
	bounceRays(rays);
	
	//TODO: Evaluate result
	return 0;
}
