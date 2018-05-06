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

class component{
public:
	virtual controlPts getControls()=0;
	virtual void setControls(const controlPts& raw)=0;
	virtual std::vector<component*> getChildren()=0;

	virtual ray checkRay(ray rin)=0;

	//for displaying on-screen

	//shorthand for drawing to full buffer
	virtual void drawTo(pbuffer &pixels);
	virtual void drawTo(pbuffer &pixels,rect &target)=0;
};

#endif // COMPONENT_H_INCLUDED
