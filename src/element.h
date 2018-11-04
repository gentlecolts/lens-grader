#ifndef ELEMENT_H
#define ELEMENT_H

#include "component.h"
#include "group.h"

//a simple circular lens
class element:public component{
protected:
	virtual rect getRect(const rect& parent);
public:
	element(group* parent);
	virtual ~element();

	std::vector<double> frontVals,backVals;
	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;

	virtual ray checkRay(ray rin) override;

	//for displaying on-screen
	virtual void drawTo(pbuffer &pixels,const rect &target) override;
};

#endif // ELEMENT_H
