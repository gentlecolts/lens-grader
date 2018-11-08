#ifndef ELEMENT_H
#define ELEMENT_H

#include "component.h"
#include "group.h"

struct circle{
	//NOTE: r is stored as r^2 for convenience
	double x0,y0,r;
	bool upper=false;
};

//a simple circular lens
class element:public component{
private:
	std::tuple<circle,circle> getFrontBack(const rect &target);
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
