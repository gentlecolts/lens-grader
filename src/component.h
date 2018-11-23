#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include "ray.h"

//TODO: use c++17 version
template<typename T>
constexpr const T& clamp(const T& v,const T& lo,const T& hi){
	return (v<lo)?lo:(hi<v)?hi:v;
}

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
	//this returns a copy, so it shoulnt be modified
	virtual const std::vector<std::shared_ptr<component>> getChildren();

	virtual ray checkRay(ray rin)=0;

	//for displaying on-screen

	//shorthand for drawing to full buffer
	virtual void drawTo(pbuffer &pixels);
	virtual void drawTo(pbuffer &pixels,const rect &target)=0;
	
	static double minThickness;
	
	//searching functions
	template <typename T>
	typename std::enable_if<std::is_base_of<component,T>::value,std::shared_ptr<T>>::type
	findChildOfType(bool deep=false){
		//while this may save a tiny bit of runtime, it's entirely redundant
		//if(children.empty()){
		//	return std::shared_ptr<T>();
		//}
		
		for(auto ptr:children){
			auto child=std::dynamic_pointer_cast<T>(ptr);
			if(child){
				return child;
			}
		}
		
		if(deep){
			for(auto ptr:children){
				auto child=ptr->findChildOfType<T>(true);
				if(child){
					return child;
				}
			}
		}
		
		return std::shared_ptr<T>();
	}
	template <typename T>
	typename std::enable_if<std::is_base_of<component,T>::value,std::list<std::shared_ptr<T>>>::type
	findChildrenOfType(bool deep=false){
		std::list<std::shared_ptr<T>> items;
		std::copy_if(children.begin(),children.end(),std::back_inserter(items),[](auto ptr)->bool{
			return std::dynamic_pointer_cast<T>(ptr);
		});
		
		if(deep){
			for(auto ptr:children){
				items.splice(items.end(),ptr->findChildrenOfType<T>(deep));
			}
		}
		
		return items;
	}
};

#endif // COMPONENT_H_INCLUDED
