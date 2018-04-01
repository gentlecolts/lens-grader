#include "group.h"
using namespace std;

group::group(){
	//ctor
}

group::~group(){
	//dtor
}

vector<double> group::getRawValues(){
	throw logic_error("Not Implemented");
}
void group::setRawValues(const vector<double>& raw){
	throw logic_error("Not Implemented");
}
vector<component*> group::getChildren(){
	throw logic_error("Not Implemented");
}

ray group::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

vector<point> group::getPoints(){
	throw logic_error("Not Implemented");
}
