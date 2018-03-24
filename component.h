#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include <vector>
#include "ray.h"

class component{
public:
	virtual std::vector<double> getRawValues()=0;
	virtual void setRawValues(const std::vector<double>& raw)=0;

	virtual ray checkRay(ray rin)=0;
};

#endif // COMPONENT_H_INCLUDED
