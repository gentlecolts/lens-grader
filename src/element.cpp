#include "element.h"
using namespace std;
element::element(){
	//ctor
}
element::~element(){
	//dtor
}

controlPts element::getControls(){
	throw logic_error("Not Implemented");
}
void element::setControls(const controlPts& raw){
	throw logic_error("Not Implemented");
}
vector<std::shared_ptr<component>> element::getChildren(){
	return vector<std::shared_ptr<component>>();
}

ray element::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

//for displaying on-screen
void element::drawTo(pbuffer &pixels,rect target){
	throw logic_error("Not Implemented");
}
