#ifndef LENS_H
#define LENS_H

#include "component.h"

//a simple circular lens
class element : public component{
public:
	std::vector<double> frontVals,backVals;
	virtual std::vector<double> getRawValues();
	virtual void setRawValues(const std::vector<double>& raw);

	virtual ray checkRay(ray rin);

	//for displaying on-screen
	virtual std::vector<point> getPoints(int resolution);
};

#endif // LENS_H
