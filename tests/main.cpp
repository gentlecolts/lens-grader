/*Provides visual tests for lens-grader
Copyright (C) 2018  Jack Phoebus

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "lens.h"
#include "group.h"
#include "element.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
using namespace std;

/*
void window::update(){
	s.update();

	SDL_RenderClear(r);
	//SDL_RenderCopy(r,tex,NULL,NULL);SDL_Event e;

	SDL_RenderCopy(r,s.tex,NULL,NULL);
	SDL_RenderPresent(r);
}//*/

SDL_Event e;
void pollEvents(){
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
		}
	}
}

template<typename Iterator>
static inline void printVector(Iterator begin, Iterator end){
	cout<<"{";
	bool second=false;
	for(; begin != end; ++begin){
		if(second){
			cout<<", ";
		}
		second=true;
		cout << *begin;
	}
	cout<<"}"<<endl;
}

int main(int argc,char** argv){
	SDL_Init(SDL_INIT_VIDEO);

	const int width=640;
	const int height=480;

	//it is presumed that this will not fail, if it does we cannot continue anyways
	SDL_Window* win=SDL_CreateWindow("Lens Grader",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_SHOWN);
	//SDL_Surface* surface=SDL_GetWindowSurface(win);
	SDL_Renderer* r=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
	SDL_Surface* surf=SDL_CreateRGBSurface(0,width,height,32,0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	pbuffer pix((uint32_t*)surf->pixels,width,height);

	SDL_Rect rec;
	rec.x=10;
	rec.y=10;
	rec.w=160;
	rec.h=100;
	
	/*
	//this pointer business isnt a requirement, but is used to make future tesing of various classes of lens objects easier
	lens* l=new lens(vector<double>{.2,.6},50);
	l->aperature=1.8;
	//component* l=new lens(2,100);
	
	auto groups=l->getGroups();
	vector<shared_ptr<element>> elms;
	elms.reserve(groups.size());
	
	//put one element in each group, store the results in our elms array
	transform(groups.begin(),groups.end(),back_inserter(elms),[](shared_ptr<group> g){
		return g->addElement<element>();
	});
	
	//set this one
	auto& elm=elms[2];
	elm->setSphereFront(.75,.7);
	//elm->setSphereBack(.25,0,.5);
	elm->setSphereBack(.4,.1);
	
	//another one
	auto elm2=elms[1];
	elm2->setSphereFront(.6,.8);
	elm2->setSphereBack(.5,.3);
	
	//and one last one for the back
	auto elm3=elms[0];
	elm3->setSphereFront(.8,.6);
	elm3->setSphereBack(.2,.3);
	
	//mess with the groups too
	l->getGroups()[0]->setWidth(1);
	l->getGroups()[2]->setWidth(1);
	l->getGroups()[0]->setMovementMultiplier(0);
	l->getGroups()[1]->setMovementMultiplier(0.5);
	l->getGroups()[2]->setMovementMultiplier(1);
	//TODO: write shape to file?
	/*/
	//test designs we've generated
	lens* l=new lens(1,50);
	
	//just one element per group
	auto groups=l->getGroups();
	vector<shared_ptr<element>> elms;
	elms.reserve(groups.size());
	
	//put one element in each group, store the results in our elms array
	transform(groups.begin(),groups.end(),back_inserter(elms),[](shared_ptr<group> g){
		return g->addElement<element>();
	});
	
	//test values
	auto refs=l->getControlRefs();
	vector<double> testctrl=
		//{0.108393, 0.982939, 1, 0.119442, 0.886951, 0.556787, 0.256375, 0.991708, 0.87531, 0.0722254, 0.961295}
		//this following design went exceptionally well, these are the steps it took
		//{0.124147, 0.956713, 0.983917, 0.165133, 0.903907, 0.549597, 0.24791, 0.965648, 0.87531, 0.0619481, 0.961295}
		//{0.113764, 0.981528, 1, 0.0699313, 0.874204, 0.553504, 0.262524, 0.996464, 0.87531, 0.0755885, 0.961295}
		//{0.116248, 0.980585, 1, 0.0367941, 0.869294, 0.550798, 0.26642, 0.999054, 0.87531, 0.0773851, 0.961295}
		//{0.116255, 0.980565, 1, 0.0366634, 0.86928, 0.550789, 0.266434, 0.999063, 0.87531, 0.0773916, 0.961295}
		
		{0.0491353, 0.942277, 1, 0.0424067, 0.814217, 0.579844, 0.303715, 1, 0.907977, 0.0735575, 0.840897}
	;
	
	cout<<"Test size: "<<testctrl.size()<<endl;
	cout<<"AvailalbeRefs: "<<refs.size()<<endl;
	for(int i=0;i<testctrl.size();i++){
		refs[i].get()=testctrl[i];
	}
	
	auto geo=l->getGeometry();
	auto& points=geo.groups[0].lenses[0];
	printVector(points.begin(),points.end());
	//*/
	
	auto startTime = chrono::system_clock::now();
	const double frequency=4*atan(1.0);
	
	auto control=l->getControls();
	printVector(control.begin(),control.end());
	l->setControls(control);
	control=l->getControls();
	printVector(control.begin(),control.end());
	
	//test copying
	auto lCopy=l->clone();
	auto copyControls=lCopy->getControls();
	copyControls[0]=.6;
	lCopy->setControls(copyControls);
	//make sure only the copy changed
	control=l->getControls();
	printVector(control.begin(),control.end());
	control=lCopy->getControls();
	printVector(control.begin(),control.end());
	
	while(1){
		//SDL_FillRect(surface,NULL,SDL_MapRGB(surface->format,0xe5,0x5c,0x69));
		//SDL_FillRect(surface,&rec,0x7f00ff);

		//*
		SDL_SetRenderDrawColor(r,0xe5,0x5c,0x69,255);
		SDL_RenderClear(r);
		SDL_SetRenderDrawColor(r,127,0,255,255);
		SDL_RenderFillRect(r,&rec);
		SDL_SetRenderDrawColor(r,0,255,0,255);
		SDL_RenderDrawLine(r,10,10,170,110);
		//*/
		
		//*move over time
		auto nowTime = chrono::system_clock::now();
		chrono::duration<double> diff = nowTime-startTime;
		
		dynamic_cast<lens*>(l)->setPosition((sin(frequency*diff.count())+1)/2);
		//*/
		
		const double scale=.5;
		SDL_FillRect(surf,NULL,0x00000000);
		l->drawTo(pix,rect(width*(0.5-scale/2),0*height*(0.5-scale/2),width*scale,height*scale));

		//SDL_UpdateTexture(SDL_GetRenderTarget(r),NULL,pixels,width*sizeof(*pixels));
		SDL_Texture* tex=SDL_CreateTextureFromSurface(r,surf);
		SDL_RenderCopy(r,tex,0,0);
		SDL_DestroyTexture(tex);

		SDL_RenderPresent(r);
		pollEvents();
	}

	return 0;
}
