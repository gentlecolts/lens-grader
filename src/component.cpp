#include "component.h"
using namespace std;

component::component(component* parentComponent):parent(parentComponent){
	/*NOTE:that this does not work, there must be at least one shared pointer to this prior to shared_from_this being called, which cannot happen by the constructor
	just use newChildComponent
	if(parentComponent){
		parentComponent->children.push_back(shared_from_this());
	}
	//*/
}

void component::drawTo(pbuffer &pixels){
	rect r(0,0,pixels.w,pixels.h);
	drawTo(pixels,r);
}

const vector<shared_ptr<component>> component::getChildren(){
	return children;
}
