#include "group.h"
#include "element.h"
#include "lens.h"
#include <algorithm>
using namespace std;

group::group(lens* parent):component(parent){
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

void group::drawTo(pbuffer &pixels,const rect &target){
	throw logic_error("Not Implemented");
}

//TODO: use c++17 version
template<typename T>
constexpr const T& clamp(const T& v,const T& lo,const T& hi){
	return (v<lo)?lo:(hi<v)?hi:v;
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
