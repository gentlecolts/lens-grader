#include "lens.h"
using namespace std;

lens::lens(std::vector<double> groupDivs,double focalLength):component(){
	//ctor
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

void lens::drawTo(pbuffer &pixels,rect target){
	const uint32_t color=0xffdacd47;
	
	//determine the actual shape of the bounding box
	const double
		cx=target.x+target.w/2,
		cy=target.y+target.h/2;
	double h=target.h,w=h/aperature;
	
	if(w>target.w){
		h=h*target.w/w;
		w=target.w;
	}
	
	//create new bounds with the correct dimensions
	target.w=w;
	target.h=h;
	target.x=cx-w/2;
	target.y=cy-h/2;

	//TODO: clip target rect to buffer's size before this loop, in case of bad input
	for(int j=target.y;j<target.y+target.h;j++){
		for(int i=target.x;i<target.x+target.w;i++){
			pixels.pixels[i+pixels.w*j]=color;
		}
	}
}
