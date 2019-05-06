#include "lens.h"
#include "group.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
using namespace std;

struct lens::drawRects{
	rect full,barrel,sensor;
};

//TODO: should this be a protected virtual of lens?
static inline double gradeRays(const std::vector<rayPath>& rays){
	double squareErr=0;
	int count=0;
	for(auto& path:rays){
		const ray& r=path.segments.back();
		if(r.lost){
			//good if the ray was supposed to be lost, bad if it wasnt, score this somehow
		}else{
			const double s=r.p.x - path.target.x;
			//cout<<ray.segments.back().p.x<<" "<<ray.target.x<<" "<<s<<endl;
			squareErr+=s*s;
			++count;
		}
	}
	squareErr/=count;
	//cout<<score<<endl;
	return isfinite(squareErr)?1/(1+squareErr):0;
}
static inline void intersectSensor(std::vector<rayPath>& rays){
	for(auto& path:rays){
		ray& r=path.segments.back();
		if(!r.lost){
			//solve y=0
			const double t=-r.p.y/r.dir.y;
			const double x=r.dir.x*t+r.p.x;
			//add in a new point
			path.refract(ray(x,0,0,1),1);
		}
	}
}

void lens::initializeControlVars(){
	//no control vars here yet
}
shared_ptr<component> lens::copy() const{
	auto lcopy=make_shared<lens>(*this);
	lcopy->initializeControlVars();
	return lcopy;
}

lens::lens():lens(0,50){
}
lens::lens(int groupCount, double focalLen):component(nullptr),focalLength(focalLen),physicalLength(focalLen){
	groupCount=max(groupCount,1);//must have at least one group
	
	for(int i=0;i<groupCount;i++){
		auto g=newChildComponent<group>(this);
		g->setFront((double(i+1))/groupCount);
		g->setBack((double(i))/groupCount);
	}
	
	initializeControlVars();
	
	cout<<"made a lens with "<<children.size()<<" children"<<endl;
}

//TODO: test for an empty vector being passed
lens::lens(std::vector<double> groupDivs,double focalLen):lens(groupDivs.size()+1,focalLen){
	//sort the dividers
	sort(groupDivs.begin(),groupDivs.end());
	
	//set each in order
	for(int i=0;i<children.size();i++){
		const double
			back=i?groupDivs[i-1]:0,
			front=(i==groupDivs.size())?1:groupDivs[i];
		auto g=dynamic_pointer_cast<group>(children[i]);
		g->setBack(back);
		g->setFront(front);
		cout<<"group from "<<back<<" to "<<front<<endl;
	}
}

lens::~lens(){
	//dtor
}

vector< rayPath > lens::initializeRays(double distanceFromFront, int resolution,int samples){
	//The rays produced by this funciton are in real world coordinates (mm), where (0,0) is the center of the image sensor
	
	vector<rayPath> rays;
	
	//TODO: is this  the correct behavior?  alternative: just use min and continue with min vals if invalid values are given
	if(resolution<2 || samples<1){
		return rays;
	}
	//negative distance would mean focusing on something inside the lens itself, we dont care to allow for that, even if might be technically possible
	distanceFromFront=max(distanceFromFront,0.0);
	
	const bool finite=isfinite(distanceFromFront);
	const double
		lensFront=sensorToBack+physicalLength,
		sampleY=finite?lensFront+distanceFromFront:focalLength*2,//if infinite, use an arbitrary sample point instead of actually being infinitely far away
		xmax=-(sampleY-focalLength)*imageCircleRadius/focalLength,
		lensHalfWidth=physicalLength/(2*aperature);
	const int sampleN=max(samples-1,1);
	const point focal(0,focalLength);
	
	//TODO: fov should probably be from the front of the glass, and not from the focal point(?) so calculate fov here and adjust the below accordingly
	//NOTE: see https://www.edmundoptics.com/resources/application-notes/imaging/understanding-focal-length-and-field-of-view/ re: principle plane
	
	for(int i=0;i<resolution;i++){
		const point p0(xmax*i/(resolution-1),sampleY);
		for(int j=0;j<samples;j++){
			//if only one sample, just cast straight through the focal point
			//otherwise, distribute the samples across the entire surface of the lens's front
			const point p1=(samples==1)?focal:point(lensHalfWidth*((2.0*j)/(samples-1)-1),lensFront);
			const ray lightray=ray().fromPoints(p0,p1);
			
			double targetX;
			if(finite){
				//finite case is easy, since we're just mapping the focal plane onto the sensor but flipped
				targetX=-imageCircleRadius*p0.x/abs(xmax);
			}else{
				//when at infinity, the start position doesnt actually matter, only the angle affects the end position
				//if you need to prove this to yourself, consider a finite sized object that is infinitely far away
				//no matter how big it is, you will only see it as a single point, due to perspective
				
				//thusly, the target x is when y=0 for the ray starting at the focal point, with a direction of the incoming ray's direction
				//it is assumed that dy will never equal zero
				//(y-y0)/dy = (x-x0)/dx
				//-y0*dx/dy+x0=x
				targetX=focal.x-focal.y*lightray.dir.x/lightray.dir.y;
			}
			
			//TODO: as of c++17, emplace_back returns a reference, should probably use that for instantiation instead
			rayPath path;
			path.segments.push_back(lightray);
			path.target=point(targetX,0);
			
			//since the at-infinity sampling method can produce rays that should not be able to reach the sensor, we need to reject those
			//TODO: that rejection should be handled by the evaluator, ie there should be some "rejected" state for the rayPath's target
			//TODO: alternatively, just change the generation method at infinity
			if(abs(targetX)<=imageCircleRadius){
				rays.push_back(path);
			}
		}
		
		//TODO: add bokeh samples too
	}
	
	return rays;
}

vector<double> lens::getControls(){
	//lens itself has no controlpoints, but it does combine the control points of its children
	vector<double> controls;
	appendChildrenControlsTo(controls);
	//printf("Total of %i control vars\n",controls.size());
	return controls;
}
vector<controlRef> lens::getControlRefs(){
	vector<controlRef> controls;
	appendChildrenControlRefsTo(controls);
	//printf("Total of %i control refs\n",controls.size());
	return controls;
}
void lens::setControls(vector<double>& controls){
	//no controls in lens, so we can just pass right along to the children
	for(auto& child:children){
		child->setControls(controls);
	}
}

void lens::bounceRays(vector< rayPath >& paths){
	//group[0] is the back of the lens, we need to iterate in reverse
	//groups are not allowed to overlap, so no need to check ordering
	for(auto r=children.rbegin();r!=children.rend();++r){
		(*r)->bounceRays(paths);
	}
}

double lens::fullWidth() const{
	return max(physicalLength/aperature,2*max(mountRadius,imageCircleRadius));
}
rect lens::getRect(const rect& parent) const{
	//since this is a "lens" we only care about the bounding box of the barrel itself.  nothing should be in or care about the sensor area outside of drawing
	auto height=physicalLength,width=physicalLength/aperature;
	const auto scale=min(parent.w/width,parent.h/height);
	height*=scale;
	width*=scale;
	
	const point center(parent.x+parent.w/2,parent.y+parent.h/2);
	
	return rect(center.x-width/2,center.y-width/2,width,height);
}
lens::drawRects lens::getDrawRects(const rect& parent) const{
	drawRects rects;
	
	//we're going to need this
	const auto fullwidth=fullWidth();
	
	//first, the big bounding box
	{
		auto width=fullwidth,height=sensorToBack+physicalLength;
		const auto scale=min(parent.w/width,parent.h/height);
		height*=scale;
		width*=scale;
		
		const point center(parent.x+parent.w/2,parent.y+parent.h/2);
		rects.full=rect(center.x-width/2,center.y-width/2,width,height);
	}
	
	//now we can get the lens bounding boxes
	{
		const auto
			lengthRatio=physicalLength/(physicalLength+sensorToBack),
			widthRatio=(physicalLength/aperature)/fullwidth;
		rects.barrel.h=rects.full.h*lengthRatio;
		rects.barrel.y=rects.full.y+rects.full.h*(1-lengthRatio);
		rects.barrel.w=rects.full.w*widthRatio;
		rects.barrel.x=rects.full.x+rects.full.w*(1-widthRatio)/2;
	}
	
	//finally the sensor
	{
		const auto widthRatio=2*max(mountRadius,imageCircleRadius)/fullwidth;
		rects.sensor.h=rects.full.h-rects.barrel.h;
		rects.sensor.y=rects.full.y;
		rects.sensor.w=rects.full.w*widthRatio;
		rects.sensor.x=rects.full.x+rects.full.w*(1-widthRatio)/2;
	}
	
	//and we're done
	return rects;
}
rect lens::getRealSize() const{
	const auto width=physicalLength/aperature;
	return rect(-width/2,sensorToBack,width,physicalLength);
}

void lens::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		lenscolor=0xffdacd47,
		sensorcolor=0xff5caee5;
	const auto drawrects=getDrawRects(target);
	auto& lensRec=drawrects.barrel;
	//cout<<lensRec.w<<" "<<lensRec.h<<" "<<lensRec.x<<" "<<lensRec.y<<endl;
	
	//TODO: clip target rect to buffer's size before this loop, in case of bad input
	
	//TODO: this does not quite correctly handle cases where the target rect is sufficiently vertical and the sensor is wider than the lens
	//solution: less hacky calculation of the lens and sensor bounding boxes early on, base calculations off of those
	
	//draw lens area
	for(int j=lensRec.y;j<(int)(lensRec.y+lensRec.h);j++){
		for(int i=lensRec.x;i<(int)(lensRec.x+lensRec.w);i++){
			pixels.pixels[i+pixels.w*flipInsideBuffer(pixels,target,j)]=lenscolor;
		}
	}
	
	//draw sensor area
	//this is a bit more interesting, a trapezoid with the top being the mount radius and the bottom being the image circle
	const auto& sensorRect=drawrects.sensor;
	const auto
		cx=sensorRect.x+sensorRect.w/2,
		sensorXRadius=max(imageCircleRadius,mountRadius),
		mountPixelRadius=sensorRect.w/2 * mountRadius/sensorXRadius,
		circlePixelRadius=sensorRect.w/2 * imageCircleRadius/sensorXRadius;
	const int j0=sensorRect.y,j1=sensorRect.y+sensorRect.h;
	
	for(int j=j0;j<j1;j++){
		//how wide should we be
		const double t=((double)(j-j0))/(j1-1-j0);
		const int realwidth=circlePixelRadius+t*(mountPixelRadius-circlePixelRadius);//note this is actually half of the width, but that's what we want anyways
		
		const int y=flipInsideBuffer(pixels,target,j);
		pixels.drawLinePixels(cx-realwidth,y,cx+realwidth,y,sensorcolor);
	}
	
	//draw each group
	//cout<<"drawing groups"<<endl;
	for(auto groupcomponent:children){
		groupcomponent->drawTo(pixels,lensRec);
	}
	
	//display rays
	//auto rays=initializeRays(numeric_limits<double>::infinity(),4,8);
	auto rays=initializeRays(100,4,3);
	bounceRays(rays);
	
	//calculate the final ray intersections with the image sensor
	intersectSensor(rays);
	
	//get the score
	//printf("Score: %f\n",gradeRays(rays));
	
	//draw just the initial segment
	//const auto halfwidth=physicalLength/(2*aperature);
	const auto pointRemap=[&](const point& p){
		const auto x=drawrects.full.x+drawrects.full.w*(1+p.x/(fullWidth()/2))/2;
		const auto y=flipInsideBuffer(pixels,target,drawrects.full.y+drawrects.full.h*p.y/(sensorToBack+physicalLength));
		
		return point(x,y);
	};
	
	//auto p0=pointRemap(point(-physicalLength/(2*aperature),physicalLength+sensorToBack)),p1=pointRemap(point(imageCircleRadius,0));
	//pixels.drawLinePixels(p0,p1);
	
	/*just draw start point and target
	for(auto& ray:rays){
		point p1=ray.segments[0].p;
		p1.x+=ray.segments[0].dir.x;
		p1.y+=ray.segments[0].dir.y;
		pixels.drawLinePixels(pointRemap(ray.segments[0].p),pointRemap(p1));
		pixels.drawLinePixels(pointRemap(p1),pointRemap(ray.target));
	}
	/*/
	//draw all points on each path, then TODO the final intersection point
	for(auto& ray:rays){
		if(ray.segments.size()>1){
			auto r0=ray.segments.begin();
			const auto rend=ray.segments.end();
			
			/*
			for_each(r0,rend,[](const auto& r){
				printf("{(%f,%f) (%f,%f)} ",r.p.x,r.p.y,r.dir.x,r.dir.y);
			});printf("\n");
			//*/
			
			for(auto r1=(r0+1);r1!=rend;++r1){
				if(!r0->lost){
					pixels.drawLinePixels(pointRemap(r0->p),pointRemap(r1->p));
					r0=r1;
				}
			}
		}
	}
	//*/
}

void lens::setPosition(double pos){
	for(auto child:children){
		if(auto g=dynamic_pointer_cast<group>(child)){
			g->setPosition(pos);
		}
	}
}

shared_ptr<group> lens::getBackGroup(){
	//children[0] is back
	return dynamic_pointer_cast<group>(children.front());
}
shared_ptr<group> lens::getFrontGroup(){
	return dynamic_pointer_cast<group>(children.back());
}
const vector<shared_ptr<group>> lens::getGroups(){
	vector<shared_ptr<group>> groups(children.size());
	std::transform(children.begin(),children.end(),groups.begin(),[](auto ptr){
		return dynamic_pointer_cast<group>(ptr);
	});
	
	return groups;
}

double lens::getScore(){
	//TODO: for each set of rays tested, we need to set the position appropriately
	auto rays=initializeRays(numeric_limits<double>::infinity(),10,10);
	bounceRays(rays);
	
	intersectSensor(rays);
	
	//TODO: Evaluate result
	return gradeRays(rays);
}

lensGeometry lens::getGeometry(){
	lensGeometry geo;
	for(auto child:children){
		if(auto g=dynamic_pointer_cast<group>(child)){
			geo.groups.push_back(g->getGeometry());
		}
	}
	geo.sensorToBack=sensorToBack;
	
	return geo;
}
