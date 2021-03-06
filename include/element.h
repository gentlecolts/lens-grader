/*declares base element component
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
	virtual rect getRect(const rect& parent) const override;
	
	std::vector<double> frontVals,backVals;
	double ior=1.485;//Polyvinyl butyral has an ior of 1.485
	
	virtual void validate();
	
	virtual void initializeControlVars() override;
	virtual std::shared_ptr<component> copy() const override;
public:
	element(group* parent);
	virtual ~element();

	virtual std::vector<double> getControls() override;
	virtual std::vector<controlRef> getControlRefs() override;
	virtual void setControls(std::vector<double>& controls) override;
	virtual rect getRealSize() const override;

	virtual void bounceRays(std::vector<rayPath>& paths) override;

	//for displaying on-screen
	virtual void drawTo(pbuffer &pixels,const rect &target) override;
	
	//virtual void setFrontValues(const std::vector<double>& vals);
	//virtual void setBackValues(const std::vector<double>& vals);
	
	void setSphereFront(double centerPos,double edgePos,double width=1);
	void setSphereBack(double centerPos,double edgePos,double width=1);
	void setSphereFrontBack(double frontCenter,double frontEdge,double backCenter,double backEdge);
	void setSphereFrontBack(double frontCenter,double frontEdge,double frontWidth,double backCenter,double backEdge,double backWidth);
	
	//TODO: should this be a compontent function instead?  can a group non-trivially invalid?
	virtual bool isValid();
	
	virtual std::vector<point> getSurface();
};

#endif // ELEMENT_H
