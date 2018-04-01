#ifndef GROUP_H
#define GROUP_H

#include "component.h"
#include "element.h"

/*group with many elements
all elements within the group move together, if at all
elements may overlap for more complex shapes
if elements overlap, accuracy cannot be guaranteed unless overlapping elements share the same index of refraction

TODO: consider specialized subclasses of group that do not allow overlap for increased performance
(lens would need to be updated to use a vector of group pointers)
*/
class group:public component{
protected:
	std::vector<element*> elements;
public:
	double
		front,back,//position of front and back of group, in lens-space
		width,//width of group, also in lens-space
		moveRange=0;//range of lens motion, as a percent

	group();
	virtual ~group();

	virtual std::vector<double> getRawValues();
	virtual void setRawValues(const std::vector<double>& raw);
	virtual std::vector<component*> getChildren();

	virtual ray checkRay(ray rin);

	virtual std::vector<point> getPoints();
};

#endif // GROUP_H
