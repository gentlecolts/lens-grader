/*Declares base lens component
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
#ifndef LENS_H
#define LENS_H

#include "component.h"

class group;
class groupGeometry;

struct lensGeometry{
	std::vector<groupGeometry> groups;
	
	//TODO: mount type?
	double sensorToBack;
};

class lens:public component{
protected:
	struct drawRects;
	
	virtual rect getRect(const rect& parent) const override;
	virtual drawRects getDrawRects(const rect& parent) const;
	virtual double fullWidth() const;
	
	//TODO: display ray count/simulation ray count should both be attributes of this class
	virtual std::vector<rayPath> initializeRays(double distanceFromFront,int resolution,int samples);
	virtual double gradeRays(const std::vector<rayPath>& rays);
	
	virtual void initializeControlVars() override;
	virtual std::shared_ptr<component> copy() const override;
public:
	//distances are in milimieters, aperature is f/a (ex f/2.8 is stored as 2.8)
	//NOTE: it doesnt seem to cause any harm having these public, but this should not be considered a final api decision
	double mountRadius=18,sensorToBack=18,imageCircleRadius=21.6333,physicalLength=100,focalLength=100,aperature=1.2;
	
	lens();
	lens(int groupCount,double focalLen);
	lens(std::vector<double> groupDivs,double focalLen);
	virtual ~lens();

	//inherited from compoent
	virtual std::vector<double> getControls() override;
	virtual std::vector<controlRef> getControlRefs() override;
	virtual void setControls(std::vector<double>& controls) override;
	virtual rect getRealSize() const override;

	void bounceRays(std::vector<rayPath>& paths) override;

	using component::drawTo;
	void drawTo(pbuffer &pixels,const rect &target) override;

	//lens specific pieces
	void setPosition(double pos);
	virtual double getScore();
	virtual std::shared_ptr<group> getFrontGroup();
	virtual std::shared_ptr<group> getBackGroup();
	virtual const std::vector<std::shared_ptr<group>> getGroups();
	
	//same as with group.h, would a subclass ever change this?
	lensGeometry getGeometry();
};

#endif // LENS_H
