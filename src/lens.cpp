#include "lens.h"
#include "group.h"
#include <iostream>
#include <algorithm>
using namespace std;


lens::lens(int groupCount, double focalLen):component(nullptr),focalLength(focalLen){
	groupCount=max(groupCount,1);//must have at least one group
	
	for(int i=0;i<groupCount;i++){
		auto g=make_shared<group>(this);
		g->setFront((double(i+1))/groupCount);
		g->setBack((double(i))/groupCount);
		children.push_back(g);
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

controlPts lens::getControls(){
	throw logic_error("Not Implemented");
}
void lens::setControls(const controlPts& raw){
	throw logic_error("Not Implemented");
}
vector<std::shared_ptr<component>> lens::getChildren(){
	throw logic_error("Not Implemented");
}

ray lens::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

void lens::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		lenscolor=0xffdacd47,
		sensorcolor=0xff5caee5;
	
	//determine the actual shape of the bounding box
	//TODO: this finds the right size for the barrel, but needs to include the sensor too
	const double
		cx=target.x+target.w/2,
		cy=target.y+target.h/2,
		ratio=(physicalLength+sensorToBack)/(physicalLength/aperature);//total length / width
	double h=target.h,w=h/ratio;
	
	//cout<<w<<" "<<h<<endl;
	
	if(w>target.w){
		h=h*target.w/w;
		w=target.w;
	}
	
	//create new bounds with the correct dimensions
	rect lensRec;
	lensRec.w=w;
	lensRec.h=h*physicalLength/(physicalLength+sensorToBack);
	lensRec.x=cx-lensRec.w/2;
	lensRec.y=
		cy-(target.h-lensRec.h)/2//center of rectangle
		-lensRec.h/2;//top of rec
	//cout<<lensRec.w<<" "<<lensRec.h<<" "<<lensRec.x<<" "<<lensRec.y<<endl;
	
	rect mountRec;
	
	
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
}
