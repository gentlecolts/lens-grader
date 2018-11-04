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
		y0=parent.y+backVal*parent.h,
		y1=parent.y+frontVal*parent.h;
	return rect(x0,y0,x1-x0,y1-y0);
}

//for displaying on-screen
void element::drawTo(pbuffer &pixels,const rect &target){
	const uint32_t
		bgcol=0xff47da83,
		bordercol=0xff007a2d;
	const auto myrec=getRect(target);
	
	for(int j=myrec.y;j<(int)(myrec.y+myrec.h);j++){
		for(int i=myrec.x;i<(int)(myrec.x+myrec.w);i++){
			pixels.pixels[i+pixels.w*j]=bgcol;
		}
	}
}
