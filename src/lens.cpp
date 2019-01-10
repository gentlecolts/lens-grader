#include "lens.h"
#include "group.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;


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

rect lens::getRect(const rect& parent) const{
	//determine the actual shape of the bounding box
	const double
	cx=parent.x+parent.w/2,
	cy=parent.y+parent.h/2,
	ratio=(physicalLength+sensorToBack)/(physicalLength/aperature);//total length / width
	double h=parent.h,w=h/ratio;
	
	//cout<<w<<" "<<h<<endl;
	
	if(w>parent.w){
		h=h*parent.w/w;
		w=parent.w;
	}
	
	//create new bounds with the correct dimensions
	rect lensRec;
	lensRec.w=w;
	lensRec.h=h*physicalLength/(physicalLength+sensorToBack);
	lensRec.x=cx-lensRec.w/2;
	lensRec.y=
		cy-(parent.h-lensRec.h)/2//center of rectangle
		-lensRec.h/2;//top of rec
	return lensRec;
}
rect lens::getRealSize() const{
	const auto width=physicalLength/aperature;
	return rect(-width/2,0,width,physicalLength);
}


void lens::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		lenscolor=0xffdacd47,
		sensorcolor=0xff5caee5;
	
	auto lensRec=getRect(target);
	//cout<<lensRec.w<<" "<<lensRec.h<<" "<<lensRec.x<<" "<<lensRec.y<<endl;
	
	//TODO: clip target rect to buffer's size before this loop, in case of bad input
	
	//TODO: this does not quite correctly handle cases where the target rect is sufficiently vertical and the sensor is wider than the lens
	//solution: less hacky calculation of the lens and sensor bounding boxes early on, base calculations off of those
	
	//draw lens area
	for(int j=lensRec.y;j<(int)(lensRec.y+lensRec.h);j++){
		for(int i=lensRec.x;i<(int)(lensRec.x+lensRec.w);i++){
			pixels.pixels[i+pixels.w*j]=lenscolor;
		}
	}
	
	//draw sensor area
	//this is a bit more interesting, a trapezoid with the top being the mount radius and the bottom being the image circle
	
	const int j0=lensRec.y+lensRec.h,j1=j0+(lensRec.h*sensorToBack)/focalLength;//j1=target.y+target.h;
	const double realtopixel=lensRec.w/(physicalLength/aperature);//pixel width of the lens divided by "real" width of the lens, as a conversion ratio point
	const double cx=target.x+target.w/2;
	
	for(int j=j0;j<j1;j++){
		//how wide should we be
		const double t=((double)(j-j0))/(j1-1-j0);
		const int realwidth=realtopixel*(mountRadius+t*(imageCircleRadius-mountRadius));//note this is actually half of the width, but that's what we want anyways
		
		for(int i=cx-realwidth;i<=cx+realwidth;i++){
			pixels.pixels[i+pixels.w*j]=sensorcolor;
		}
	}
	
	//draw each group
	//cout<<"drawing groups"<<endl;
	for(auto groupcomponent:children){
		groupcomponent->drawTo(pixels,lensRec);
	}
	
	//display rays
	auto rays=initializeRays(numeric_limits<double>::infinity(),4,8);
	//auto rays=initializeRays(100,4,3);
	bounceRays(rays);
	
	//draw just the initial segment
	//const auto halfwidth=physicalLength/(2*aperature);
	const auto
		pixelWidth=lensRec.w*imageCircleRadius/(physicalLength/(2*aperature)),
		circlePixelX=lensRec.x+lensRec.w/2-pixelWidth/2,
		pixelHeight=lensRec.h*(1+sensorToBack/focalLength);
	const auto pointRemap=[&](const point& p){
		//first convert the input point from real-world coordinates into into the coordinate space such that x=[-1,1], y=[0,1]
		//noting that x spans the image circle's width and y is from the sensor to the front of the camera
		//then convert it to the display surface space (noting that (0,0) is top-left)
		return point(circlePixelX+pixelWidth*((p.x/imageCircleRadius)+1)/2,lensRec.y+pixelHeight*(1-p.y/(physicalLength+sensorToBack)));
	};
	
	//auto p0=pointRemap(point(-physicalLength/(2*aperature),physicalLength+sensorToBack)),p1=pointRemap(point(imageCircleRadius,0));
	//pixels.drawLinePixels(p0,p1);
	
	//*just draw start point and target
	for(auto& ray:rays){
		point p1=ray.segments[0].p;
		p1.x+=ray.segments[0].dir.x;
		p1.y+=ray.segments[0].dir.y;
		pixels.drawLinePixels(pointRemap(ray.segments[0].p),pointRemap(p1));
		pixels.drawLinePixels(pointRemap(p1),pointRemap(ray.target));
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
	auto rays=initializeRays(numeric_limits<double>::infinity(),10,10);
	bounceRays(rays);
	
	//TODO: Evaluate result
	return 0;
}
