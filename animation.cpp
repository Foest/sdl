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
const int DOT_WIDTH = 20;

//Globals
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = {0, 0, 0};

//Prototypes
struct Circle;
bool init();
double distance(int x1, int y1, int x2, int y2);
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL);
SDL_Surface *load_image(std::string filename);
bool load_files();
bool check_collision(Circle &A, Circle &B);
bool check_collision(Circle &A, std::vector<SDL_Rect> &B);
void clean_up();

//Structs/Classes
struct Circle
{
  int x, y;
  int r;
};

class Foo
{
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

class Dot
{
  private:
    Circle c;
    int xVel, yVel;

  public:
      Dot();
      void handle_input();
      void move(std::vector<SDL_Rect> &rects, Circle &circle);
      void show();
};

//Functions
int main(int argc, char* args[])
{
  bool quit = false;
  bool cap = true;
  Timer fps;
  Dot myDot;
  std::vector<SDL_Rect> box(1);
  Circle otherDot;

  box[0].x = 60;
  box[0].y = 60;
  box[0].w = 40;
  box[0].h = 40;

  otherDot.x = 30;
  otherDot.y = 30;
  otherDot.r = DOT_WIDTH / 2;

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
      myDot.move(box, otherDot);
      SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
      SDL_FillRect(screen, &box[0], SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
      apply_surface(otherDot.x - otherDot.r, otherDot.y - otherDot.r, dot, screen);
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

double distance(int x1, int y1, int x2, int y2)
{
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
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

  SDL_WM_SetCaption("Circular Collision Detection", NULL);

  return true;
}

bool load_files()
{
  dot = load_image("dot.bmp");

  font = TTF_OpenFont("lazy.ttf", 30);

  if(dot == NULL)
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
  SDL_FreeSurface(dot);

  TTF_CloseFont(font);

  TTF_Quit();
  SDL_Quit();
}

bool check_collision(Circle &A, Circle &B)
{
  if(distance(A.x, A.y, B.x, B.y) < (A.r + B.r))
  {
    return true;
  }
  return false;
}

bool check_collision(Circle &A, std::vector<SDL_Rect> &B)
{
  int cX, cY;

  for(int Bbox = 0; Bbox < B.size(); Bbox++)
  {
    if(A.x < B[Bbox].x)
    {
      cX = B[Bbox].x;
    }
    else if( A.x > B[Bbox].x + B[Bbox].w)
    {
      cX = B[Bbox].x + B[Bbox].w;
    }
    else
    {
      cX = A.x;
    }

    if(A.y < B[Bbox].y)
    {
      cY = B[Bbox].y;
    }
    else if( A.y > B[Bbox].y + B[Bbox].h)
    {
      cY = B[Bbox].y + B[Bbox].h;
    }
    else
    {
      cY = A.y;
    }

    if(distance(A.x, A.y, cX, cY) < A.r)
    {
      return true;
    }
  }
  return false;
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

Dot::Dot()
{
  c.x = DOT_WIDTH / 2;
  c.y = DOT_WIDTH / 2;
  c.r = DOT_WIDTH / 2;
  xVel = 0;
  yVel = 0;
}


void Dot::handle_input()
{
  if(event.type == SDL_KEYDOWN)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_UP: yVel -= 1; break;
      case SDLK_DOWN: yVel += 1; break;
      case SDLK_LEFT: xVel -= 1; break;
      case SDLK_RIGHT: xVel += 1; break;
    }
  }
  else if(event.type == SDL_KEYUP)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_UP: yVel += 1; break;
      case SDLK_DOWN: yVel -= 1; break;
      case SDLK_LEFT: xVel += 1; break;
      case SDLK_RIGHT: xVel -= 1; break;
    }
  }
}

void Dot::move(std::vector<SDL_Rect> &rects, Circle &circle)
{
  c.x += xVel;
  if((c.x - DOT_WIDTH / 2 < 0 ) || (c.x + DOT_WIDTH / 2 > SCREEN_WIDTH) || (check_collision(c, rects)) || (check_collision(c, circle)))
  {
    c.x -= xVel;
  }

  c.y += yVel;

  if(( c.y - DOT_WIDTH / 2 < 0 ) || (c.y + DOT_WIDTH / 2 > SCREEN_HEIGHT) || (check_collision(c, rects)) || (check_collision(c, circle)))
  {
    c.y -= yVel;
  }
}

void Dot::show()
{
  apply_surface(c.x - c.r, c.y - c.r, dot, screen);
}
