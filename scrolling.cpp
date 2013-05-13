#include "iostream"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

//Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;
const int FOO_WIDTH = 64;
const int FOO_HEIGHT = 205;
const int FOO_RIGHT = 0;
const int FOO_LEFT = 1;
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

//Globals
SDL_Surface *dot = NULL;
SDL_Surface *background = NULL;
SDL_Surface *foo = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
SDL_Rect clipsLeft[4];
SDL_Rect clipsRight[4];
SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

//Prototypes
struct Circle;
bool init();
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL);
SDL_Surface *load_image(std::string filename);
bool load_files();
void clean_up();
void set_clips();

//Structs/Classes
struct Circle
{
  int x, y;
  int r;
};

class Foo
{
  private:
    int offSet;
    int velocity;
    int frame;
    int status;

  public:
    Foo();
    void handle_events();
    void move();
    void show();
};

class Dot
{
  private:
    int x, y;
    int xVel, yVel;
  public:
    Dot();
    void handle_input();
    void move();
    void show();
    void set_camera();
};
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

//Functions
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
  Timer fps;
  Foo walk;

  //While user hasn't quit
  while(quit == false)
  {
    fps.start();

    while(SDL_PollEvent(&event))
    {
      walk.handle_events();

      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
    walk.move();
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
    walk.show();

    if(SDL_Flip(screen) == -1)
    {
      return 1;
    }

    if(fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
    {
      SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
    }
  }

  clean_up();
  return 0;
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip)
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

  SDL_WM_SetCaption("Animation", NULL);

  return true;
}

bool load_files()
{
  foo = load_image("foo.png");

  if(foo == NULL)
  {
    return false;
  }

  return true;
}

void clean_up()
{
  SDL_FreeSurface(foo);

  TTF_Quit();
  SDL_Quit();
}

void set_clips()
{
  clipsRight[ 0 ].x = 0;
  clipsRight[ 0 ].y = 0;
  clipsRight[ 0 ].w = FOO_WIDTH;
  clipsRight[ 0 ].h = FOO_HEIGHT;
  clipsRight[ 1 ].x = FOO_WIDTH;
  clipsRight[ 1 ].y = 0;
  clipsRight[ 1 ].w = FOO_WIDTH;
  clipsRight[ 1 ].h = FOO_HEIGHT;
  clipsRight[ 2 ].x = FOO_WIDTH * 2;
  clipsRight[ 2 ].y = 0;
  clipsRight[ 2 ].w = FOO_WIDTH;
  clipsRight[ 2 ].h = FOO_HEIGHT;
  clipsRight[ 3 ].x = FOO_WIDTH * 3;
  clipsRight[ 3 ].y = 0;
  clipsRight[ 3 ].w = FOO_WIDTH;
  clipsRight[ 3 ].h = FOO_HEIGHT;
  clipsLeft[ 0 ].x = 0;
  clipsLeft[ 0 ].y = FOO_HEIGHT;
  clipsLeft[ 0 ].w = FOO_WIDTH;
  clipsLeft[ 0 ].h = FOO_HEIGHT;
  clipsLeft[ 1 ].x = FOO_WIDTH;
  clipsLeft[ 1 ].y = FOO_HEIGHT;
  clipsLeft[ 1 ].w = FOO_WIDTH;
  clipsLeft[ 1 ].h = FOO_HEIGHT;
  clipsLeft[ 2 ].x = FOO_WIDTH * 2;
  clipsLeft[ 2 ].y = FOO_HEIGHT;
  clipsLeft[ 2 ].w = FOO_WIDTH;
  clipsLeft[ 2 ].h = FOO_HEIGHT;
  clipsLeft[ 3 ].x = FOO_WIDTH * 3;
  clipsLeft[ 3 ].y = FOO_HEIGHT;
  clipsLeft[ 3 ].w = FOO_WIDTH;
  clipsLeft[ 3 ].h = FOO_HEIGHT;
}

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

Foo::Foo()
{
  offSet = 0;
  velocity = 0;
  frame = 0;
  status = FOO_RIGHT;
}

void Foo::handle_events()
{
  if(event.type == SDL_KEYDOWN)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_RIGHT: velocity += FOO_WIDTH / 4; break;
      case SDLK_LEFT: velocity -= FOO_WIDTH / 4; break;
    }
  }
  else if(event.type == SDL_KEYUP)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_RIGHT: velocity -= FOO_WIDTH / 4; break;
      case SDLK_LEFT: velocity += FOO_WIDTH / 4; break;
    }
  }
}

void Foo::move()
{
  offSet += velocity;

  if((offSet < 0) || (offSet + FOO_WIDTH > SCREEN_WIDTH))
  {
    offSet -= velocity;
  }
}

void Foo::show()
{
  if(velocity < 0)
  {
    status = FOO_LEFT;
    frame++;
  }
  else if(velocity > 0)
  {
    status = FOO_RIGHT;
    frame++;
  }
  else
  {
    frame = 0;
  }

  if(frame >= 4)
  {
    frame = 0;
  }

  if(status == FOO_RIGHT)
  {
    apply_surface(offSet, SCREEN_HEIGHT - FOO_HEIGHT, foo, screen, &clipsRight[frame]);
  }
  else if(status = FOO_LEFT)
  {
    apply_surface(offSet, SCREEN_HEIGHT - FOO_HEIGHT, foo, screen, &clipsLeft[frame]);
  }
}
