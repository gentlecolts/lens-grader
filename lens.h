#ifndef LENS_H
#define LENS_H

#include "component.h"
#include "group.h"

class lens:public component{
protected:
	std::vector<group> groups;

	//mount radius in meters, only used for conversions
	double mountRadius;

	//these distances are stored all as proportional to mount radius
	//ex: if mount radius is 35mm and one of these is 7cm, they should be stored as 2
	double sensorToBack=1,imageCircleRadius=1,width=1,length=1;

public:
	lens(std::vector<double> groupDivs,double focalLength);
	virtual ~lens();

	//inherited from compoent
	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;
	std::vector<std::shared_ptr<component>> getChildren() override;

	ray checkRay(ray rin) override;

	void drawTo(pbuffer &pixels,rect &target) override;

	//lens specific pieces
	//virtual double getError();
};

#endif // LENS_H
