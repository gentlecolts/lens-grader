#ifndef LENS_H
#define LENS_H

#include "component.h"
#include "group.h"

class lens:public component{
public:
	std::vector<group> groups;

	lens();
	virtual ~lens();

	std::vector<double> getRawValues();
	void setRawValues(const std::vector<double>& raw);
	std::vector<component*> getChildren();

	ray checkRay(ray rin);

	std::vector<point> getPoints();

};

#endif // LENS_H
