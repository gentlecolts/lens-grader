#ifndef LENS_H
#define LENS_H

#include "component.h"
#include "group.h"

class lens:public component{
protected:
	std::vector<group> groups;

	//distances are in milimieters, aperature is f/a (ex f/2.8 is stored as 2.8)
	double mountRadius=1,sensorToBack=1,imageCircleRadius=1,aperature=2.8,physicalLength=1,focalLength=1;

public:
	lens(std::vector<double> groupDivs,double focalLength);
	virtual ~lens();

	//inherited from compoent
	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;
	std::vector<std::shared_ptr<component>> getChildren() override;

	ray checkRay(ray rin) override;

	void drawTo(pbuffer &pixels,rect target) override;

	//lens specific pieces
	//virtual double getError();
};

#endif // LENS_H
