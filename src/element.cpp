#include "element.h"
#include <algorithm>
#include <cmath>
using namespace std;

void element::initializeControlVars(){
	//make sure we dont have anything in the vector
	controlVars.clear();
	//and set it to the proper values
	for(double& val:frontVals){
		appendControlVar(val);
	}
	for(double& val:backVals){
		appendControlVar(val);
	}
}
shared_ptr<component> element::copy() const{
	auto ecopy=make_shared<element>(*this);
	ecopy->initializeControlVars();
	return ecopy;
}

element::element(group* parent):component(parent){
	//defaults
	setSphereFrontBack(.6,.65,1,.5,.45,1);
	
	initializeControlVars();
}
element::~element(){
	//dtor
}

vector<double> element::getControls(){
	/*
	auto controls=copyControlVars();
	printf("Element added %ui control vars\n",(uint)controls.size());
	return controls;
	/*/
	return copyControlVars();
	//*/
}
vector<controlRef> element::getControlRefs(){
	return vector<controlRef>(controlVars.begin(),controlVars.end());
}
void element::setControls(vector<double>& controls){
	consumeControlVars(controls);
}

template <typename T> static constexpr int sgn(T val){
    return (T(0)<val)-(val<T(0));
}

template<typename T>
static constexpr T square(const T& x){
	return x*x;
}
static inline auto circleIntersection(const circle &e,const ray &r){
	/*
	x=dx*t+x0
	y=dy*t+y0
	(x-cx0)^2+(y-cy0)^2=r^2
	
	(dx*t+x0-cx0)^2+(dy*t+y0-cy0)^2=r^2
	(dx*t)^2+2*dx*t*(x0-cx0)+(x0-cx0)^2+(dy*t)^2+2*(dy*t)*(y0-cy0)+(y0-cy0)^2=r^2
	(dx^2+dy^2)*t^2+(2*dx*(x0-cx0)+2*dy*(y0-cy0))*t+(x0-cx0)^2+(y0-cy0)^2-r^2=0
	*/
	const double
		a=square(r.dir.x)+square(r.dir.y),
		b=2*(r.dir.x*(r.p.x-e.x0)+r.dir.y*(r.p.y-e.y0)),
		c=square(r.p.x-e.x0)+square(r.p.y-e.y0)-e.r,
		//if dy is positive: upper shell uses greater t, lower shell uses lower t
		//if dy is negative: upper shell uses lower t, lower shell uses upper t
		//NOTE: dy "should" never actually be positive in our simplified model, but must be accounted for
		//NOTE: this also doesnt guarentee in general that a hit will always be the correct shell,
		//however the edge cases of this should be handled by the geometry of the lens
		//consider: if both intersections are  on the bottom shell.  the only possible way for this to happen is that either
		//A) the ray had to have passed through the barrel of the lens
		//B) the ray will have first hit the flat plane of the lens, and that intersection will be used instead anyways
		//since neither of these are issues, we can ignore this case
		//TODO: additionally, since we know the ray must hit the circle by the time we get here (see refractCircle), we probably dont even need any if checks, and probably dont need to return t either
		s=(e.upper?1:-1)*sgn(r.dir.y),
		t=(-b+s*sqrt(b*b-4*a*c))/(2*a);
	if(isfinite(t)){
		const point p0(e.x0,e.y0),p1(r.dir.x*t+r.p.x,r.dir.y*t+r.p.y);
		ray normal=ray().fromPoints(p0,p1).normalized();
		normal.p=p1;//make sure it's coming out from the surface (point of intersection), not the circle's center
		return make_tuple(t,normal);
	}else{
		return make_tuple(t,ray());
	}
}

static inline void refractCircle(vector<rayPath>& paths,const double planeY,const double xmin,const double xmax,const rect& bounds,const circle& circle,const double ior){
	for(rayPath& path:paths){
		//first intersect the front plane
		const ray& r=path.segments.back();
		double t=(planeY-r.p.y)/r.dir.y;
		auto x=r.dir.x*t+r.p.x;
		ray normal(x,planeY,0,1);//normal is a ray pointing up from the point of intersection
		
		if(x>xmin && x<xmax){//should be using the circle instead
			tie(t,normal)=circleIntersection(circle,r);
		}else if(x<bounds.x || x>bounds.x+bounds.h){//missed the plane entirely
			t=-numeric_limits<double>::infinity();
			
			ray rcopy=r;
			rcopy.lost=true;
			path.segments.push_back(rcopy);
		}
		
		//if elements are overlapping, it's possible for t to be negative
		//NOTE: this will need to be updated to correctly handle more than one glass material at the same time
		if(t>=0){
			path.refract(normal,ior);
		}
	}
}

static inline auto getXminmax(const rect& r,const double width){
	return make_tuple(r.x+r.w*(1-width)/2,r.x+r.w*(1+width)/2);
}

void element::bounceRays(vector< rayPath >& paths){
	//printf("element bouncerays\n");
	
	//get our surfaces in real-world coords
	circle frontCircle,backCircle;
	const auto bounds=getRealSize();
	std::tie(frontCircle,backCircle)=getFrontBack(bounds);
	
	//printf("Real Bounds: [%f, %f, %f, %f]\n",bounds.x,bounds.y,bounds.w,bounds.h);
	
	//check the front element, if it intersects at t<0 then ignore the intersection
	double t,xmin,xmax;
	ray normal;
	tie(xmin,xmax)=getXminmax(bounds,frontVals[2]);
	const auto fronty=bounds.y+frontVals[1]*bounds.h;
	
	//TODO: for generality and reusability, this should instead use the actual surface geometry from getSurface() and trace against that.  this will be more expensive, but more accurate as well
	refractCircle(paths,fronty,xmin,xmax,bounds,frontCircle,ior);
	
	
	//TODO: after the back intersection is calculated, calculate the intensity loss between the front and back of the lens
	//NOTE: alternatively, implement some material struct and compute fresnel stuff in refract()
	
	//check the back element, same as front
	//*
	tie(xmin,xmax)=getXminmax(bounds,backVals[2]);
	const auto backy=bounds.y+backVals[1]*bounds.h;
	refractCircle(paths,backy,xmin,xmax,bounds,backCircle,1);
	//*/
}


rect element::getRect(const rect& parent) const{
	//1 = front, 0 = back
	const auto
		frontVal=*max_element(begin(frontVals),end(frontVals)),
		backVal=*min_element(begin(backVals),end(backVals));
	//TODO: 1.0 should be the greatest width
	auto w=1.0*parent.w;
	
	auto
		x0=parent.x+parent.w/2 - w/2,
		x1=x0+w,
		y0=parent.y+backVal*parent.h,
		y1=parent.y+frontVal*parent.h;
	return rect(x0,y0,x1-x0,y1-y0);
}
rect element::getRealSize() const{
	const auto r=getRect(parent->getRealSize());
	//printf("Element is bounded by: (%f, %f), (%f, %f)\n",r.x,r.y,r.w,r.h);
	return r;
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
	
	return make_tuple(circle1,circle2);
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
			j0=flipInsideBuffer(pixels,target,y0),
			j1=flipInsideBuffer(pixels,target,y1);
		if(j0>=j1){
			//FIXME: whatever this was debugging
			//printf("%i j=(%i %i) y=(%f %f) front=<%f %f %f> back=<%f %f %f>\n",i,j0,j1,y0,y1,frontX0,frontX1,frontY,backX0,backX1,backY);
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
	validate();
}
void element::setSphereFront(double centerPos, double edgePos,double width){
	frontVals={centerPos,edgePos,width};
	validate();
}
void element::setSphereFrontBack(double frontCenter, double frontEdge, double backCenter, double backEdge){
	setSphereFrontBack(frontCenter,frontEdge,frontVals[2],backCenter,backEdge,backVals[2]);
}
void element::setSphereFrontBack(double frontCenter, double frontEdge, double frontWidth, double backCenter, double backEdge, double backWidth){
	frontVals={frontCenter,frontEdge,frontWidth};
	backVals={backCenter,backEdge,backWidth};
	validate();
}


void element::validate(){
	//TODO: this function isnt really runtime ready, but it can work in a pinch
	for(auto front=frontVals.begin(),back=backVals.begin(); front != frontVals.end() && back != backVals.end(); ++front, ++back){
		//0=back, 1=front
		
		//back cannot be in front of the front
		if(*front<*back){
			iter_swap(front,back);
		}
		
		//cannot be thinner than the min thickness, but also may not be outside the 0-1 range
		//TODO: enforce minimum thickness?  NOTE: need to check both x and y thicknesses?  x will always be thin somewhere, is that ok?
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

static inline void addSurfaceTo(vector<point>& points,double xmin,double xmax,const circle& circle,bool reverse=false){
	const int n=100;
	const double dx=(xmax-xmin)/(n-1);
	const int sign=circle.upper?1:-1;
	for(int i=0;i<n;i++){
		const double x=reverse?xmax-i*dx:xmin+i*dx;
		const double y=circle.y0+sign*sqrt(circle.r-square(x-circle.x0));
		points.push_back(point(x,y));
	}
}
vector<point> element::getSurface(){
	vector<point> points;
	//get our spheres in real-world coords
	circle frontCircle,backCircle;
	const auto bounds=getRealSize();
	double xmin,xmax;
	tie(frontCircle,backCircle)=getFrontBack(bounds);
	
	//start with the front surface, going left to right
	double y=bounds.y+frontVals[1]*bounds.h;
	tie(xmin,xmax)=getXminmax(bounds,frontVals[2]);
	
	//TODO: lambda or static inline pls
	y=frontCircle.y0+(frontCircle.upper?1:-1)*sqrt(frontCircle.r-square(xmax-frontCircle.x0));
	
	//if y is nan, then (probably from rounding error) x^2>r^2, but the intent was probably that x^2==r^2
	if(!isfinite(y)){y=frontCircle.y0;}
	
	points.push_back(point(bounds.x,y));
	points.push_back(point(xmin,y));
	addSurfaceTo(points,xmin,xmax,frontCircle);
	points.push_back(point(xmax,y));
	points.push_back(point(bounds.x+bounds.w,y));
	
	//then the back surface, but backwards, since we want to make a loop, we do it right to left
	y=bounds.y+backVals[1]*bounds.h;
	tie(xmin,xmax)=getXminmax(bounds,backVals[2]);
	
	//if((backCircle.upper && y<backCircle.y0) || (!backCircle.upper && y>backCircle.y0)){//this is just xnor
		//y=backCircle.y0;
		y=backCircle.y0+(backCircle.upper?1:-1)*sqrt(backCircle.r-square(xmax-backCircle.x0));
	//}
	if(!isfinite(y)){y=backCircle.y0;}
	
	points.push_back(point(bounds.x+bounds.w,y));
	points.push_back(point(xmax,y));
	addSurfaceTo(points,xmin,xmax,backCircle,true);
	points.push_back(point(xmin,y));
	points.push_back(point(bounds.x,y));
	
	//and we're done
	return points;
}
