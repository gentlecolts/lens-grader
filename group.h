#ifndef GROUP_H
#define GROUP_H

#include "component.h"
#include "element.h"

//basic, one-element group
class group:public component{
private:
	element elm;
public:
	group();
	virtual ~group();

	std::vector<double> getRawValues();
	void setRawValues(const std::vector<double>& raw);
	std::vector<component*> getChildren();

	ray checkRay(ray rin);

	std::vector<point> getPoints();
};

#endif // GROUP_H
