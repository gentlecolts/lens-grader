#include "group.h"
using namespace std;

group::group(){
	//ctor
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

void group::drawTo(pbuffer &pixels,rect target){
	throw logic_error("Not Implemented");
}
