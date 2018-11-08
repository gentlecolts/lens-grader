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

ray element::checkRay(ray rin){
	throw logic_error("Not Implemented: "+string(__func__));
}

rect element::getRect(const rect& parent){
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
		myrec.x+myrec.w
	);
	circle2.y0=calcY0(
		c2y1,
		myrec.y+backVals[1]*myrec.h,
		circle2.x0,
		myrec.x+myrec.w
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
	
	circle circle1,circle2;
	std::tie(circle1,circle2)=getFrontBack(target);
	
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
		return c.y0+(c.upper?1:-1)*sqrt(c.r-x*x);
	};
	
	for(int j=myrec.y;j<(int)(myrec.y+myrec.h);j++){
		for(int i=myrec.x;i<(int)(myrec.x+myrec.w);i++){
			//if((circleContains(circle1,i,j)^flipC1) && (circleContains(circle2,i,j)^flipC2)){
			const int x=i,y=(target.y+target.h)-1-(j-target.y);
			
			if(y<=circleY(circle1,x) && y>=circleY(circle2,x)){
				pixels.pixels[i+pixels.w*j]=bgcol;
			}
		}
	}
}
