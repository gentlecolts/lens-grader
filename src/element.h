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
	virtual rect getRect(const rect& parent);
	
	std::vector<double> frontVals,backVals;
	
	virtual void validate();
public:
	element(group* parent);
	virtual ~element();

	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;

	virtual std::vector<ray> checkRays(const std::vector<ray>& rin) override;

	//for displaying on-screen
	virtual void drawTo(pbuffer &pixels,const rect &target) override;
	
	//virtual void setFrontValues(const std::vector<double>& vals);
	//virtual void setBackValues(const std::vector<double>& vals);
	
	void setSphereFront(double centerPos,double edgePos);
	void setSphereBack(double centerPos,double edgePos);
	void setSphereFrontBack(double frontCenter,double frontEdge,double backCenter,double backEdge);
	
	//TODO: should this be a compontent function instead?  can a group non-trivially invalid?
	virtual bool isValid();
};

#endif // ELEMENT_H
