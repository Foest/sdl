#include "SDL/SDL.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *load_image(std::string filename)
{
  //Temporary storage for the image that's loaded
  SDL_Surface *loadedImage = NULL;

  //Optimized image that will be used
  SDL_Surface *optimizedImage = NULL;

  //Load the image
  loadedImage = SDL_LoadBMP(filename.c_str());

  //If nothing went wrong in loading the image
  if(loadedImage != NULL)
  {
    //Create an optimized image
    optimizedImage = SDL_DisplayFormat(loadedImage);

    //Free the old image
    SDL_FreeSurface(loadedImage);
  }

  //Return the optimized image
  return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination)
{
  //Make a temporary rectangle to hold the offsets
  SDL_Rect offset;

  //Give the offsets to the rectangle
  offset.x = x;
  offset.y = y;

  //Blit the surface
  SDL_BlitSurface(source, NULL, destination, &offset);
}

int main(int argc, char* args[])
{
  //The images
  SDL_Surface* hello = NULL;
  SDL_Surface* screen = NULL;

  //Start SDL
  SDL_Init(SDL_INIT_EVERYTHING);

  //Set up screen
  screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);

  //Load image
  hello = SDL_LoadBMP("hello.bmp");

  //Apply image to screen
  SDL_BlitSurface(hello, NULL, screen, NULL);

  //Update screen
  SDL_Flip(screen);

  //Pause
  SDL_Delay(2000);

  //Free the loaded image
  SDL_FreeSurface(hello);

  //Quit SDL
  SDL_Quit();

  return 0;
}
