#ifndef LENS_H
#define LENS_H

#include "component.h"

class group;
class groupGeometry;

struct lensGeometry{
	std::vector<groupGeometry> groups;
	//TODO: mount type?
};

class lens:public component{
protected:
	//distances are in milimieters, aperature is f/a (ex f/2.8 is stored as 2.8)
	double mountRadius=17,sensorToBack=46.50,imageCircleRadius=21.6333,physicalLength=100,focalLength=100,aperature=2.8;
	
	struct drawRects;
	
	virtual rect getRect(const rect& parent) const override;
	virtual drawRects getDrawRects(const rect& parent) const;
	virtual double fullWidth() const;
	
	//TODO: display ray count/simulation ray count should both be attributes of this class
	virtual std::vector<rayPath> initializeRays(double distanceFromFront,int resolution,int samples);
	
public:
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
