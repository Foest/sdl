#include "iostream"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib>

//Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;
const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;
const int TOTAL_PARTICLES = 20;

//Globals
SDL_Surface *dot = NULL;
SDL_Surface *shimmer= NULL;
SDL_Surface *blue= NULL;
SDL_Surface *green= NULL;
SDL_Surface *red= NULL;
SDL_Surface *back = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
SDL_Color textColor = {0xFF, 0xFF, 0xFF};

//Structs/Classes
struct Circle
{
  int x, y;
  int r;
};

class Particle
{
  private:
    int x, y;
    int frame;

    SDL_Surface *type;

  public:
    Particle(int X, int Y);
    void show();
    bool is_dead();
};

class Dot
{
  private:
    int x, y;
    int xVel, yVel;
    Particle *particles[TOTAL_PARTICLES];

  public:
    Dot();
    ~Dot();
    void handle_input();
    void move();
    void show();
    void show_particles();
    void set_x(int X);
    void set_y(int Y);
    int get_x();
    int get_y();
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

//Prototypes
struct Circle;
bool init();
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL);
SDL_Surface *load_image(std::string filename);
bool load_files();
void clean_up();

//Functions
int main(int argc, char* args[])
{
  int alpha = SDL_ALPHA_OPAQUE;
  Timer fps;
  Dot myDot;
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

  //While user hasn't quit
  while(quit == false)
  {

    fps.start();

    while(SDL_PollEvent(&event))
    {
      myDot.handle_input();

      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }


    myDot.move();
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
    myDot.show();

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

  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

  if(screen == NULL)
  {
    return false;
  }

  SDL_WM_SetCaption("Particle Engine", NULL);

  srand(SDL_GetTicks());

  return true;
}

bool load_files()
{
  dot = load_image("dot.bmp");

  if(dot == NULL)
  {
    return false;
  }

  red = load_image("red.bmp");
  green = load_image("green.bmp");
  blue = load_image("blue.bmp");
  shimmer = load_image("shimmer.bmp");

  if((shimmer == NULL) || (red == NULL) || (green == NULL) || (blue == NULL))
  {
    return false;
  }

  SDL_SetAlpha(red, SDL_SRCALPHA | SDL_RLEACCEL, 192);
  SDL_SetAlpha(blue, SDL_SRCALPHA | SDL_RLEACCEL, 192);
  SDL_SetAlpha(green, SDL_SRCALPHA | SDL_RLEACCEL, 192);
  SDL_SetAlpha(shimmer, SDL_SRCALPHA | SDL_RLEACCEL, 192);

  return true;
}

void clean_up()
{
  SDL_FreeSurface(dot);
  SDL_FreeSurface(red);
  SDL_FreeSurface(green);
  SDL_FreeSurface(blue);
  SDL_FreeSurface(shimmer);

  SDL_Quit();
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

void Dot::handle_input()
{
  if(event.type == SDL_KEYDOWN)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_UP: yVel -= DOT_HEIGHT / 2; break;
      case SDLK_DOWN: yVel += DOT_HEIGHT / 2; break;
      case SDLK_RIGHT: xVel += DOT_WIDTH/ 2; break;
      case SDLK_LEFT: xVel -= DOT_WIDTH/ 2; break;
    }
  }
  else if(event.type == SDL_KEYUP)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_UP: yVel += DOT_HEIGHT / 2; break;
      case SDLK_DOWN: yVel -= DOT_HEIGHT / 2; break;
      case SDLK_RIGHT: xVel -= DOT_WIDTH/ 2; break;
      case SDLK_LEFT: xVel += DOT_WIDTH/ 2; break;
    }
  }
}

Dot::Dot()
{
  x = 0;
  y = 0;
  xVel = 0;
  yVel = 0;

  for(int p = 0; p < TOTAL_PARTICLES; p++)
  {
    particles[p] = new Particle(x, y);
  }
}

void Dot::move()
{
  x += xVel;

  if((x < 0) || (x + DOT_WIDTH > SCREEN_WIDTH))
  {
    x -= xVel;
  }

  y += yVel;

  if((y < 0) || (y + DOT_HEIGHT > SCREEN_HEIGHT))
  {
    y -= yVel;
  }
}

Dot::~Dot()
{
  for (int p = 0; p < TOTAL_PARTICLES; p++)
  {
    delete particles[p];
  }
}

void Dot::show()
{
  apply_surface(x, y, dot, screen);
  show_particles();
}

void Dot::show_particles()
{
  for(int p = 0; p < TOTAL_PARTICLES; p++)
  {
    if(particles[p]->is_dead() == true);
    {
      delete particles[p];
      particles[p] = new Particle(x, y);
    }
  }

  for(int p = 0; p < TOTAL_PARTICLES; p++)
  {
    particles[p]->show();
  }
}

void Dot::set_x(int X)
{
  x = X;
}

void Dot::set_y(int Y)
{
  y = Y;
}

int Dot::get_x()
{
  return x;
}

int Dot::get_y()
{
  return y;
}

Particle::Particle(int X, int Y)
{
  x = X - 5 + (rand() % 25);
  y = Y - 5 + (rand() % 25);

  frame = rand() % 5;

  switch(rand() % 3)
  {
    case 0: type = red; break;
    case 1: type = green; break;
    case 2: type = blue; break;
  }
}

void Particle::show()
{
  apply_surface(x, y, type, screen);

  if(frame % 2 == 0)
  {
    apply_surface(x, y, shimmer, screen);
  }

  frame++;
}

bool Particle::is_dead()
{
  if(frame > 10)
  {
    return true;
  }

  return false;
}
