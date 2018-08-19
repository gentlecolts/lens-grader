#ifndef GROUP_H
#define GROUP_H

#include "component.h"

class element;
class lens;

/*group with many elements
all elements within the group move together, if at all
elements may overlap for more complex shapes
if elements overlap, accuracy cannot be guaranteed unless overlapping elements share the same index of refraction

TODO: consider specialized subclasses of group that do not allow overlap for increased performance
(lens would need to be updated to use a vector of group pointers)
*/
class group:public component{
protected:
	std::vector<std::shared_ptr<element>> elements;
	
	//getters/setters are used to ensure constraint conformance (including resizing adjacent elements)
	double
		front=1,back=0,//position of front and back of group, as a percent of the lens's length [0-1]
		width=1,//width of group, as a percent of the lens's width [0-1]
		range=0,//range of group motion, as a percent [0-1]
		position=0;//position along move range, as a percent;
	/*NOTE: range does not affect the actual front or back.  the wider the move range, the smaller the actual lens space is.
	the distance between the real front and back could be seen as 1-range.  when the range is 0, the lens elements have the full 0 to 1 range to be placed in, with no movemenet allowed, at 1 the elements would only have a thin area to move in, but could shift along the entire range
	
	range=0
	==Group=Front==
	|****element***
	|****element***
	|****element***
	|****element***
	|****element***
	|****element***
	==Group=Back===
	
	
	range=0.5
	==Group=Front==
	|    ^
	|shift space
	|    v
	|****element***
	|****element***
	|****element***
	==Group=Back===
	
	if this were not the case, and instead the range was additional padding outside of the space allocated by front and back, then defining range as a 0-1 value would be much more inconvenient, and require a lot more extra logic to resize overlapping groups
	*/
	
	//any time the front/back/range is changed, this should be called to push any other 
	virtual void reevaluateDims();
public:
	group(lens* parent);
	virtual ~group();

	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;
	virtual std::vector<std::shared_ptr<component>> getChildren() override;

	virtual ray checkRay(ray rin) override;

	virtual void drawTo(pbuffer &pixels,const rect &target) override;
	
	virtual void setFront(double f);
	virtual void setBack(double b);
	virtual void setWidth(double w);
	virtual void setRange(double r);
	virtual void setPosition(double p);
	
	virtual double getFront();
	virtual double getBack();
	virtual double getWidth();
	virtual double getRange();
	virtual double getPosition();
};

#endif // GROUP_H
