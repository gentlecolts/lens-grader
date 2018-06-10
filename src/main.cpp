#include "lens.h"
#include <SDL2/SDL.h>
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

	component* l=new lens(vector<double>{.2,.6},100);

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

		l->drawTo(pix);

		//SDL_UpdateTexture(SDL_GetRenderTarget(r),NULL,pixels,width*sizeof(*pixels));
		SDL_Texture* tex=SDL_CreateTextureFromSurface(r,surf);
		SDL_RenderCopy(r,tex,0,0);
		SDL_DestroyTexture(tex);

		SDL_RenderPresent(r);
		pollEvents();
	}

	return 0;
}