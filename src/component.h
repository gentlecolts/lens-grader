#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include <vector>
#include <memory>
#include <stdexcept>
#include "ray.h"

struct controlPts{
	std::vector<double> pts;
	std::vector<std::unique_ptr<controlPts>> children;
};

class component:public std::enable_shared_from_this<component>{
protected:
	//this is kept raw intentionally, for a few reasons
	//1) destroying this should not ever attempt to delete the parent
	//2) more importantly, to prevent any sort of odd circular ownerships for reference counting
	//this object will be destroyed when its parent component is anyways
	//TODO: what if the parent is destroyed while someone else holds a reference to this?
	component* parent;
	std::vector<std::shared_ptr<component>> children;
	
	virtual rect getRect(const rect &parent)=0;
	
	template <typename T,class... Args>
	typename std::enable_if<std::is_base_of<component,T>::value,std::shared_ptr<T>>::type
	newChildComponent(Args&&... args){
		auto child=std::make_shared<T>(std::forward<Args>(args)...);
		children.push_back(child);
		return child;
	}
public:
	component(component* parentComponent);
	
	virtual controlPts getControls()=0;
	virtual void setControls(const controlPts& raw)=0;
	virtual std::vector<std::shared_ptr<component>> getChildren();

	virtual ray checkRay(ray rin)=0;

	//for displaying on-screen

	//shorthand for drawing to full buffer
	virtual void drawTo(pbuffer &pixels);
	virtual void drawTo(pbuffer &pixels,const rect &target)=0;
};

#endif // COMPONENT_H_INCLUDED
