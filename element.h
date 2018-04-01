#ifndef ELEMENT_H
#define ELEMENT_H

#include "component.h"

//a simple circular lens
class element:public component{
public:
	std::vector<double> frontVals,backVals;
	virtual std::vector<double> getRawValues();
	virtual void setRawValues(const std::vector<double>& raw);
	std::vector<component*> getChildren();

	virtual ray checkRay(ray rin);

	//for displaying on-screen
	virtual std::vector<point> getPoints();
};

#endif // ELEMENT_H
