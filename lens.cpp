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
vector<component*> lens::getChildren(){
	throw logic_error("Not Implemented");
}

ray lens::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

void lens::drawTo(pbuffer &pixels,rect &target){
	throw logic_error("Not Implemented");
}
