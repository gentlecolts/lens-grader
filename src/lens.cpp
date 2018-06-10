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

void lens::drawTo(pbuffer &pixels,rect &target){
	const uint32_t color=0xffdacd47;

	//TODO: clip target rect to buffer's size before this loop, in case of bad input
	for(int j=target.y;j<target.y+target.h;j++){
		for(int i=target.x;i<target.x+target.w;i++){
			pixels.pixels[i+pixels.w*j]=color;
		}
	}
}
