#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;
SDL_Surface *dots = NULL;
SDL_Event event;
SDL_Rect clip[4];

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

  //If the image was optimized without error
  if(optimizedImage != NULL)
  {
    //Map the color key
    Uint32 colorkey = SDL_MapRGB(optimizedImage->format, 0, 0xFF, 0xFF);

    //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
    SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
  }


  //Return the optimized image
  return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL)
{
  //Make a temporary rectangle to hold the offsets
  SDL_Rect offset;

  //Give the offsets to the rectangle
  offset.x = x;
  offset.y = y;

  //Blit the surface
  SDL_BlitSurface(source, clip, destination, &offset);
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

  SDL_WM_SetCaption("Clipping", NULL);

  return true;
}

bool load_files()
{
  //Load image
  dots = load_image("dots.png");

  //If there was an error loading the images
  if(dots == NULL)
  {
    return false;
  }

  return true;
}

void clean_up()
{
  //Free the images
  SDL_FreeSurface(dots);

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

  //Clip range for the top left
  clip[0].x=0;
  clip[0].y=0;
  clip[0].w=100;
  clip[0].h=100;

  //Clip range for the top right
  clip[1].x=100;
  clip[1].y=0;
  clip[1].w=100;
  clip[1].h=100;

  //Clip range for the bottom left
  clip[2].x=0;
  clip[2].y=100;
  clip[2].w=100;
  clip[2].h=100;

  //Clip range for the bottom left
  clip[3].x=100;
  clip[3].y=100;
  clip[3].w=100;
  clip[3].h=100;

  //Fill screen white
  SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

  //Apply the surface to the screen
  apply_surface(0, 0, dots, screen, &clip[0]);
  apply_surface(540, 0, dots, screen, &clip[1]);
  apply_surface(0, 380, dots, screen, &clip[2]);
  apply_surface(540, 380, dots, screen, &clip[3]);

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
