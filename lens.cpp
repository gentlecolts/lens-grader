#include "lens.h"
using namespace std;

lens::lens(std::vector<double> groupDivs,double focalLength):component(){
	//ctor
}

lens::~lens(){
	//dtor
}

vector<double> lens::getRawValues(){
	throw logic_error("Not Implemented");
}
void lens::setRawValues(const vector<double>& raw){
	throw logic_error("Not Implemented");
}
vector<component*> lens::getChildren(){
	throw logic_error("Not Implemented");
}

ray lens::checkRay(ray rin){
	throw logic_error("Not Implemented");
}

vector<point> lens::getPoints(){
	throw logic_error("Not Implemented");
}
