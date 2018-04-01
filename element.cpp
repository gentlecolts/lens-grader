#include "element.h"
using namespace std;
element::element(){
	//ctor
}
element::~element(){
	//dtor
}

vector<double> element::getRawValues(){
	throw logic_error("Not Implemented");
}
void element::setRawValues(const vector<double>& raw){
	throw logic_error("Not Implemented");
}
vector<component*> element::getChildren(){
	return vector<component*>();
}

ray element::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

//for displaying on-screen
vector<point> element::getPoints(){
	throw logic_error("Not Implemented");
}
