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

	virtual std::vector<double> getRawValues();
	virtual void setRawValues(const std::vector<double>& raw);
	virtual std::vector<component*> getChildren();

	virtual ray checkRay(ray rin);

	virtual std::vector<point> getPoints();
};

#endif // GROUP_H
