#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

struct point{
	double x,y;
	point();
	point(double x0,double y0);
};

struct ray{
	point p,dir;
	ray();
	ray(const point& origin,const point& direction);
	ray(double x,double y,double dx,double dy);

	void fromPoints(const point& p0,const point& p1);
	void fromPoints(double x0,double y0,double x1,double y1);
};

#endif // RAY_H_INCLUDED
