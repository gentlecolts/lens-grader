#ifndef LENS_H
#define LENS_H

#include "component.h"
#include "group.h"

class lens:public component{
protected:
	std::vector<group> groups;
public:
	lens(std::vector<double> groupDivs,double focalLength);
	virtual ~lens();

	//inherited from compoent
	std::vector<double> getRawValues();
	void setRawValues(const std::vector<double>& raw);
	std::vector<component*> getChildren();

	ray checkRay(ray rin);

	std::vector<point> getPoints();

};

#endif // LENS_H
