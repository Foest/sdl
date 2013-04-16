#include "iostream"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <sstream>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;

SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *message= NULL;

SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = {0, 0, 0};

class Timer
{
  private:
    int startTicks;
    int pausedTicks;

    bool paused;
    bool started;

  public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    int get_ticks();
    bool is_started();
    bool is_paused();
};

Timer::Timer()
{
  startTicks = 0;
  pausedTicks = 0;
  paused = false;
  started = false;
}

void Timer::start()
{
  started = true;
  paused = false;
  startTicks = SDL_GetTicks();
}

void Timer::stop()
{
  started = false;
  paused = false;
}

int Timer::get_ticks()
{
  if(started == true)
  {
    if(paused == true)
    {
      return pausedTicks;
    }
    else
    {
      return SDL_GetTicks() - startTicks;
    }
  }
  return 0;
}

void Timer::pause()
{
  if((started == true) && (paused == false))
    {
      paused = true;
      pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
  if(paused == true)
  {
    paused = false;
    startTicks = SDL_GetTicks() - pausedTicks;
    pausedTicks = 0;
  }
}

bool Timer::is_started()
{
  return started;
}

bool Timer::is_paused()
{
  return paused;
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

  SDL_WM_SetCaption("FRAME RATE TESTING", NULL);

  return true;
}

bool load_files()
{
  //Load image
  background = load_image("background.png");

  //Open the font
  font = TTF_OpenFont("lazy.ttf", 30);

  //If there was an error loading the images
  if(background == NULL)
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
  SDL_FreeSurface(message);

  TTF_CloseFont(font);

  TTF_Quit();
  SDL_Quit();
}



int main(int argc, char* args[])
{
  bool quit = false;
  int frame = 0;
  bool cap = true;
  Timer fps;

  if(init() == false)
  {
    return 1;
  }

  //Load the files
  if(load_files() == false)
  {
    return 1;
  }

  Timer myTimer;

  message = TTF_RenderText_Solid(font, "Testing Frame Rate", textColor);

  myTimer.start();

  //While user hasn't quit
  while(quit == false)
  {
    fps.start();

    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_KEYDOWN)
      {
        if(event.key.keysym.sym == SDLK_RETURN)
        {
          cap = (!cap);
        }
      }
      else if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
      apply_surface(0, 0, background, screen);
      apply_surface((SCREEN_WIDTH - message->w) / 2, ((SCREEN_HEIGHT + message->h * 2) / FRAMES_PER_SECOND) * (frame % FRAMES_PER_SECOND) - message->h, message, screen);

      if(SDL_Flip(screen) == -1)
      {
        return 1;
      }

      frame++;

      if((cap == true) && (fps.get_ticks() < 1000 / FRAMES_PER_SECOND))
      {
        SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
      }
  }

  clean_up();
  return 0;
}
