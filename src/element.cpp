#include "element.h"
#include <algorithm>
using namespace std;

element::element(group* parent):component(parent){
	//ctor
}
element::~element(){
	//dtor
}

controlPts element::getControls(){
	throw logic_error("Not Implemented: "+string(__func__));
}
void element::setControls(const controlPts& raw){
	throw logic_error("Not Implemented: "+string(__func__));
}

void element::bounceRays(vector< rayPath >& paths){
	//printf("element bouncerays\n");
	
	//get our surfaces in real-world coords
	circle frontCircle,backCircle;
	std::tie(frontCircle,backCircle)=getFrontBack(getRealSize());
	
	//check the front element, if it intersects at t<0 then ignore the intersection
	//TODO
	
	//check the back element, same as front
	//TODO
}


rect element::getRect(const rect& parent) const{
	//1 = front, 0 = back
	const auto
		frontVal=*max_element(begin(frontVals),end(frontVals)),
		backVal=*min_element(begin(backVals),end(backVals));
	
	auto w=1.0*parent.w;
	
	auto
		x0=parent.x+parent.w/2 - w/2,
		x1=x0+w,
		//since the rect coords are lower=front/top, higher=back/bottom, gotta flip it
		y0=parent.y+(1-frontVal)*parent.h,
		y1=parent.y+(1-backVal)*parent.h;
	return rect(x0,y0,x1-x0,y1-y0);
}
rect element::getRealSize() const{
	return getRect(parent->getRealSize());
}


tuple< circle, circle > element::getFrontBack(const rect &myrec){
	/*(x-x0)^2+(y-y0)^2=r^2
	 * at x=x1=x0,y=y1
	 * (y1-y0)^2=r^2
	 * 
	 * at x=x2,y=y2
	 * (x2-x0)^2+(y2-y0)^2=r^2
	 * A=(x2-x0)^2
	 * A+(y2-y0)^2=r^2
	 * 
	 * (y1-y0)^2=A+(y2-y0)^2
	 * y1^2-2*y1*y0+y0^2=A+y2^2-2*y2*y0+y0^2
	 * 
	 * A+y2^2-2*y2*y0+y0^2-y1^2+2*y1*y0-y0^2=0
	 * A+y2^2-y1^2-2*y2*y0+2*y1*y0=0
	 * A+y2^2-y1^2-2*(y2-y1)*y0=0
	 * A+y2^2-y1^2=2*(y2-y1)*y0
	 * (A+y2^2-y1^2)/(2*(y2-y1))=y0
	 */
	//TODO: with the addition of the width paramenter, consider fitting to an elipse instead
	//HINT: the left/right/middle points on the edge are known, if this is not enough then the elipse center could be defined as the middle of the left/right points
	circle circle1,circle2;
	
	circle1.x0=circle2.x0=myrec.x+myrec.w/2;
	
	//TODO with c++17, declaring lambdas constexpr [the expression []()constexpr{} ] becomes valid
	auto calcY0=[](auto y1,auto y2,auto x0,auto x2){
		return ((x2-x0)*(x2-x0)+y2*y2-y1*y1)/(2*(y2-y1));
	};
	
	const auto
		c1y1=myrec.y+frontVals[0]*myrec.h,
		c2y1=myrec.y+backVals[0]*myrec.h;
	circle1.y0=calcY0(
		c1y1,
		myrec.y+frontVals[1]*myrec.h,
		circle1.x0,
		myrec.x+myrec.w*(1+frontVals[2])/2
	);
	circle2.y0=calcY0(
		c2y1,
		myrec.y+backVals[1]*myrec.h,
		circle2.x0,
		myrec.x+myrec.w*(1+backVals[2])/2
	);
	
	circle1.r=(c1y1-circle1.y0);
	circle1.r*=circle1.r;
	
	circle2.r=(c2y1-circle2.y0);
	circle2.r*=circle2.r;
	
	circle1.upper=frontVals[0]>frontVals[1];
	circle2.upper=backVals[0]>backVals[1];
	
	return tuple<circle,circle>{circle1,circle2};
}

//for displaying on-screen
void element::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		bgcol=0xff47dacb,
		bordercol=0xff007a6a;
	auto myrec=getRect(target);
	
	circle frontCircle,backCircle;
	std::tie(frontCircle,backCircle)=getFrontBack(target);
	
	//TODO
	const bool
		flipC1=frontVals[0]<frontVals[1],
		flipC2=backVals[0]>backVals[1];
	auto circleContains=[](circle& circle,auto x,auto y){
		return (x-circle.x0)*(x-circle.x0)+(y-circle.y0)*(y-circle.y0)<=circle.r;
	};
	
	auto circleY=[](circle& c,auto x){
		//(x-x0)^2+(y-y0)^2=r^2
		//y=y0 +- sqrt(r^2-(x-x0)^2)
		x-=c.x0;//x=x-x0
		const auto a=max(c.r-x*x,0.0);
		//const auto r=sqrt(c.r);
		return c.y0+(c.upper?1:-1)*sqrt(a);
		//return c.y0+(c.upper?1:-1)*r*sin(acos(x/r));
	};
	
	auto square=[](const double x){
		return x*x;
	};
	const auto
		frontX0=target.x+target.w*(1-frontVals[2])/2,
		frontX1=target.x+target.w*(1+frontVals[2])/2,
		frontY=target.y+frontVals[1]*target.h,
		backX0=target.x-target.w*(1-backVals[2])/2,
		backX1=target.x+target.w*(1+backVals[2])/2,
		backY=target.y+backVals[1]*target.h;
	/*Y and j are flipped around, correct them
	y=(target.y+target.h)-1-(j-target.y)
	-y+(target.y+target.h)-1+target.y=j
	j=((target.y+target.h)-1+target.y) - y
	this can be stored in a constant, so lets do that*/
	const auto yFlip=(target.y+target.h)-1+target.y;
	
	for(int i=myrec.x;i<(int)(myrec.x+myrec.w);i++){
		auto y0=(i<=frontX0 || i>=frontX1)?frontY:circleY(frontCircle,i);
		auto y1=(i<=backX0 || i>=backX1)?backY:circleY(backCircle,i);
		
		const int
			j0=yFlip-y0,
			j1=yFlip-y1;
		if(j0>=j1){
			printf("%i j=(%i %i) y=(%f %f) front=<%f %f %f> back=<%f %f %f>\n",i,j0,j1,y0,y1,frontX0,frontX1,frontY,backX0,backX1,backY);
		}
		for(int j=j0;j<=j1;j++){
			//TODO: check bounds n stuff, or clip them above
			pixels.pixels[i+pixels.w*j]=bgcol;
		}
	}
	
	//printf("finished element drawto (%f, %f, %f, %f) in rect (%f, %f, %f, %f)\n",frontVals[0],frontVals[1],backVals[0],backVals[1],myrec.x,myrec.y,myrec.w,myrec.h);
}

void element::setSphereBack(double centerPos, double edgePos,double width){
	backVals={centerPos,edgePos,width};
	//validate();
}
void element::setSphereFront(double centerPos, double edgePos,double width){
	frontVals={centerPos,edgePos,width};
	//validate();
}
void element::setSphereFrontBack(double frontCenter, double frontEdge, double backCenter, double backEdge){
	setSphereFrontBack(frontCenter,frontEdge,1,backCenter,backEdge,1);
}
void element::setSphereFrontBack(double frontCenter, double frontEdge, double frontWidth, double backCenter, double backEdge, double backWidth){
	frontVals={frontCenter,frontEdge,frontWidth};
	backVals={backCenter,backEdge,backWidth};
	//validate();
}


void element::validate(){
	//TODO: this function isnt really runtime ready, but it can work in a pinch
	for(auto front=frontVals.begin(),back=backVals.begin(); front != frontVals.end() && back != backVals.end(); ++front, ++back){
		//0=back, 1=front
		
		//cannot be in front of the front
		if(*front<*back){
			iter_swap(front,back);
		}
		
		//cannot be thinner than the min thickness, but also may not be outside the 0-1 range
		//TODO: enforce minimum thickness?
		//TODO: what do we do if expanding to min thickness pushes our lens outside of the 0-1 range?
		
		//no values are permitted to be outside of the 0-1 range
		*front=clamp(*front,0.0,1.0);
		*back=clamp(*back,0.0,1.0);
	}
}

bool element::isValid(){
	for(auto front=frontVals.begin(),back=backVals.begin(); front != frontVals.end() && back != backVals.end(); ++front, ++back){
		if(*front<*back+minThickness || *front>1 || *front<0 || *back>1 || *back<0){
			return false;
		}
	}
	return true;
}

