#ifndef LENS_H
#define LENS_H

#include "component.h"
#include "group.h"

class lens:public component{
protected:
	std::vector<group> groups;
public:
	lens(std::vector<double> groupDivs,double focalLength);
	virtual ~lens();

	//inherited from compoent
	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;
	std::vector<component*> getChildren() override;

	ray checkRay(ray rin) override;

	void drawTo(pbuffer &pixels,rect &target) override;
};

#endif // LENS_H
