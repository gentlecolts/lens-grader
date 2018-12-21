#ifndef LENS_H
#define LENS_H

#include "component.h"

class group;

class lens:public component{
protected:
	//distances are in milimieters, aperature is f/a (ex f/2.8 is stored as 2.8)
	double mountRadius=17,sensorToBack=46.50,imageCircleRadius=21.6333,physicalLength=100,focalLength=100,aperature=2.8;
	
	virtual rect getRect(const rect& parent) const override;
	
	//TODO: display ray count/simulation ray count should both be attributes of this class
	virtual std::vector<rayPath> initializeRays(double distanceFromFront,int resolution,int samples);
	
public:
	lens(int groupCount,double focalLen);
	lens(std::vector<double> groupDivs,double focalLen);
	virtual ~lens();

	//inherited from compoent
	virtual controlPts getControls() override;
	virtual void setControls(const controlPts& raw) override;
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
};

#endif // LENS_H
