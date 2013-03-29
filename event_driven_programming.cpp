#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;
SDL_Surface *image = NULL;
SDL_Event event;

SDL_Surface *load_image(std::string filename)
{
  //Temporary storage for the image that's loaded
  SDL_Surface *loadedImage = NULL;

  //Optimized image that will be used
  SDL_Surface *optimizedImage = NULL;

  //Load the image
  loadedImage = IMG_Load(filename.c_str());

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

bool init()
{
  //Init SDL subsystems
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    return false;
  }

  //Set up screen
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

  //If there was an error setting up the screen
  if(screen == NULL)
  {
    return false;
  }

  SDL_WM_SetCaption("Event test", NULL);

  return true;
}

bool load_files()
{
  //Load image
  image = load_image("x.png");

  //If there was an error
  if(image == NULL)
  {
    return false;
  }

  return true;
}

void clean_up()
{
  //Free the image
  SDL_FreeSurface(image);

  //Quit SDL
  SDL_Quit();
}



int main(int argc, char* args[])
{
  bool quit = false;

  if(init() == false)
  {
    return 1;
  }

  //Load the files
  if(load_files() == false)
  {
    return 1;
  }

  //Apply the surface to the screen
  apply_surface(0, 0, image, screen);

  //Update screen
  if(SDL_Flip(screen) == -1)
  {
    return 1;
  }

  //While user hasn't quit
  while(quit == false)
  {
    while(SDL_PollEvent(&event))
    {
      //If the user has Xed out the window
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
  }

  //Free the surface and quit
  clean_up();

  return 0;
}
