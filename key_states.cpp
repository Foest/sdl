#include "iostream"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *up = NULL;
SDL_Surface *down= NULL;
SDL_Surface *left= NULL;
SDL_Surface *right= NULL;
SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = {0, 0, 0};

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


void Button::show()
{
  apply_surface(box.x, box.y, buttonSheet, screen, clip);
}

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

  if(TTF_Init() == -1)
  {
    return false;
  }

  SDL_WM_SetCaption("MOUSE EVENTS!!", NULL);

  return true;
}

bool load_files()
{
  //Load image
  background = load_image("background.png");
  buttonSheet = load_image("button.png");

  //Open the font
  font = TTF_OpenFont("lazy.ttf", 28);

  //If there was an error loading the images
  if(background == NULL || buttonSheet == NULL)
  {
    return false;
  }

  if(font == NULL)
  {
    return false;
  }

  return true;
}

void clean_up()
{
  //Free the images
  SDL_FreeSurface(background);
  SDL_FreeSurface(buttonSheet);

  TTF_CloseFont(font);

  TTF_Quit();

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

  set_clips();

  Button myButton(170, 120, 320, 240);

  //While user hasn't quit
  while(quit == false)
  {
    if(SDL_PollEvent(&event))
    {
      myButton.handle_events();

      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }

  SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

  myButton.show();

  if(SDL_Flip(screen) == -1)
  {
    return 1;
  }

  }

  clean_up();
  return 0;
}
