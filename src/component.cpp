#include "component.h"
using namespace std;

double component::minThickness=0;

void component::appendControlVar(double& var){
	controlVars.push_back(var);
}

vector<double> component::copyControlVars(){
	return vector<double>(controlVars.begin(),controlVars.end());
}

void component::appendChildrenControlsTo(vector<double>& target){
	for(auto child:children){
		const auto c=child->getControls();
		target.insert(target.end(),c.begin(),c.end());
	}
}
void component::appendChildrenControlRefsTo(vector<controlRef>& target){
	for(auto child:children){
		const auto c=child->getControlRefs();
		target.insert(target.end(),c.begin(),c.end());
	}
}

void component::consumeControlVars(vector<double>& source){
	const auto N=controlVars.size();
	
	//copy our new values over
	for(int i=0;i<N;i++){
		controlVars[i].get()=source[i];
	}
	
	//remove them from the source
	//std::vector<decltype(myvector)::value_type>(myvector.begin()+N, myvector.end()).swap(myvector);
	vector<double>(source.begin()+N,source.end()).swap(source);
}

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

shared_ptr<component> component::clone() const{
	//create a naive clone
	auto newComponent=this->copy();
	
	//the children of this will just be pointers to the original one's children, we need to make a deep copy
	/*
	auto originalChildren=children;
	newComponent->children.clear();
	for(const auto& child:originalChildren){
		auto newchild=child->clone();
		newComponent->children.push_back(newchild);
		newchild->parent=newComponent.get();
		newchild->initializeControlVars();//NOTE: this should be handled already in the children
	}
	/*/
	for(int i=0;i<newComponent->children.size();i++){
		auto newchild=newComponent->children[i]->clone();
		newComponent->children[i]=newchild;
		newchild->parent=newComponent.get();
		//newchild->initializeControlVars();//NOTE: this should be handled already in the children
	}
	//*/
	
	return newComponent;
}