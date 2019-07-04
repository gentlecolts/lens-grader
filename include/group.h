/*Declares base group component
Copyright (C) 2018  Jack Phoebus

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef GROUP_H
#define GROUP_H

#include "component.h"

class element;
class lens;

struct groupGeometry{
	//these are the values at position=0
	std::vector<std::vector<point>> lenses;
	//offset by this much when position=1
	double travel;
};

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
	
	virtual rect getFullRect(const rect& parent) const;
	virtual rect getRect(const rect& parent) const override;
	
	
	//getters/setters are used to ensure constraint conformance (including resizing adjacent elements)
	double
		front=1,back=0,//position of front and back of group, as a percent of the lens's length [0-1]
		width=.75,//width of group, as a percent of the lens's width [0-1]
		range=0,//range of group motion, as a percent [0-1]
		position=0,//position along move range, as a percent.  NOT SERIALIZED
		movementMultiplier=1;//a value of 1 is normal movemnt, 0.5 is no movement, and 0 flips the movement direction, values inbetween are valid and will result in a more restricted movement range
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
	
	virtual void initializeControlVars() override;
	virtual std::shared_ptr<component> copy() const override;
public:
	group(lens* parent);
	virtual ~group();

	virtual std::vector<double> getControls() override;
	virtual std::vector<controlRef> getControlRefs() override;
	virtual void setControls(std::vector<double>& controls) override;
	virtual rect getRealSize() const override;

	virtual void bounceRays(std::vector<rayPath>& paths) override;

	virtual void drawTo(pbuffer &pixels,const rect &target) override;
	
	virtual void setFront(double f);
	virtual void setBack(double b);
	virtual void setWidth(double w);
	virtual void setRange(double r);
	virtual void setPosition(double p);
	virtual void setMovementMultiplier(double m);
	
	virtual double getFront();
	virtual double getBack();
	virtual double getWidth();
	virtual double getRange();
	virtual double getPosition();
	virtual double getMovementMultiplier();
	
	template <typename T,class... Args>
	typename std::enable_if<std::is_base_of<element,T>::value,std::shared_ptr<T>>::type
	addElement(){
		return newChildComponent<T>(this);
	}
	
	//TODO: virtual?  what if a subclass needs new geometry properties?  and if not, what more could a subclass need that this doesnt provide
	groupGeometry getGeometry();
};

#endif // GROUP_H
