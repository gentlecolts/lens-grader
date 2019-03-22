#include "lens.h"
#include "group.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

struct lens::drawRects{
	rect full,barrel,sensor;
}

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

lens::lens(int groupCount, double focalLen):component(nullptr),focalLength(focalLen){
	groupCount=max(groupCount,1);//must have at least one group
	
	for(int i=0;i<groupCount;i++){
		auto g=newChildComponent<group>(this);
		g->setFront((double(i+1))/groupCount);
		g->setBack((double(i))/groupCount);
	}
	cout<<"made a lens with "<<children.size()<<" children"<<endl;
}


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

controlPts lens::getControls(){
	throw logic_error("Not Implemented: "+string(__func__));
}
void lens::setControls(const controlPts& raw){
	throw logic_error("Not Implemented: "+string(__func__));
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
	//start with the width/height of the rect
	auto width=fullWidth(),height=physicalLength+sensorToBack;
	
	//scale them down to fit in the parent rect
	const auto scale=(width/height>parent.w/parent.h)?parent.w/width:parent.h/height;
	width*=scale;
	height*=scale;
	
	//find the origin of the rect (bottom-left)
	const auto
		x=parent.x+parent.w/2-width/2,
		y=parent.y+parent.h/2-height/2;
	return rect(x,y,width,height);
}
rect lens::getBarrelRect(const rect& parent) const{
	//start with the total bounding rect
	auto r=getRect(parent);
	
	//move it up by the flange distance
	r.y+=r.h*sensorToBack/(physicalLength+sensorToBack);
	//shrink the height
	r.h*=physicalLength/(physicalLength+sensorToBack);
	
	//use the width of the barrel, instead of the total width including sensor
	const auto xmid=r.x+r.w/2,barrelWidth=physicalLength/aperature;
	r.w*=barrelWidth/fullWidth();
	r.x=xmid-r.w/2;
	
	return r;
}
rect lens::getSensorRect(const rect& parent) const{
	//start with the total bounding rect
	auto r=getRect(parent);
	
	//shrink the height
	r.h*=sensorToBack/(physicalLength+sensorToBack);
	
	//use the width of the sensor, instead of the total width including barrel
	const auto xmid=r.x+r.w/2,sensorWidth=2*max(imageCircleRadius,mountRadius);
	r.w*=sensorWidth/fullWidth();
	r.x=xmid-r.w/2;
	
	return r;
}
rect lens::getRealSize() const{
	//TODO: picking one or the other here has the problem of not necessarily matching up with the width set by getRect().  there is no good way around this, so perhaps it might be better to abandon the idea of a bounding box and instead use coordinate spaces
	//NOTE: using cordinate spaces instead of bounding boxes loses a lot of obvious promises, and may make drawing more difficult
	//const auto width=physicalLength/aperature;
	const auto width=2*imageCircleRadius;
	const rect r(-width/2,0,width,physicalLength+sensorToBack);
	printf("Lens is bounded by: (%f, %f), (%f, %f)\n",r.x,r.y,r.w,r.h);
	return r;
}

void lens::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		lenscolor=0xffdacd47,
		sensorcolor=0xff5caee5;
	
	auto lensRec=getBarrelRect(target);
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
	const auto sensorRect=getSensorRect(target);
	const auto
		cx=sensorRect.x+sensorRect.w/2,
		sensorXRadius=max(imageCircleRadius,mountRadius),
		mountPixelRadius=sensorRect.w/2*mountRadius/sensorXRadius,
		circlePixelRadius=sensorRect.w/2*imageCircleRadius/sensorXRadius;
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
	printf("Score: %f\n",gradeRays(rays));
	
	//draw just the initial segment
	//const auto halfwidth=physicalLength/(2*aperature);
	const auto
		pixelWidth=lensRec.w*imageCircleRadius/(physicalLength/(2*aperature)),
		circlePixelX=lensRec.x+lensRec.w/2-pixelWidth/2,
		pixelHeight=lensRec.h*(1+sensorToBack/focalLength);
	const auto realRec=getRealSize(),pixelRec=getRect(target);
	const auto pointRemap=[&](const point& p){
		//first convert the input point from real-world coordinates into into the coordinate space such that x=[-1,1], y=[0,1]
		//noting that x spans the image circle's width and y is from the sensor to the front of the camera
		//then convert it to the display surface space (noting that (0,0) is top-left)
		//return point(circlePixelX+pixelWidth*((p.x/imageCircleRadius)+1)/2,lensRec.y+pixelHeight*(1-p.y/(physicalLength+sensorToBack)));
		
		//TODO: real size only accounts for the width of the image circle, getRect uses the widest part of the lens (barrel, mount, or image circle).  this will probably fail if the circle is not the widest part
		const auto x=pixelRec.x+pixelRec.w*(p.x-realRec.x)/realRec.w;
		const auto y=flipInsideBuffer(pixels,target,pixelRec.y+pixelRec.h*(p.y-realRec.y)/realRec.h);
		
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
			
			//*
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
