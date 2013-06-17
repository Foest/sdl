#include "iostream"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

//Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;
const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

//Globals
SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
TTF_Font *font = NULL;
SDL_Color textColor = {0xFF, 0xFF, 0xFF};

//Prototypes
struct Circle;
bool init();
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL);
SDL_Surface *load_image(std::string filename);
bool load_files();
void clean_up();

//Structs/Classes
struct Circle
{
  int x, y;
  int r;
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

class StringInput
{
  private:
    std::string str;
    SDL_Surface *text;

  public:
    StringInput();
    ~StringInput();
    void handle_input();
    void show_centered();
};

//Functions
int main(int argc, char* args[])
{
  bool quit = false;
  bool nameEntered = false;

  if(init() == false)
  {
    return 1;
  }

  //Load the files
  if(load_files() == false)
  {
    return 1;
  }

  StringInput name;
  message = TTF_RenderText_Solid(font, "New High Score! Enter Name:", textColor);

  //While user hasn't quit
  while(quit == false)
  {
    while(SDL_PollEvent(&event))
    {

      if(nameEntered == false)
      {
        name.handle_input();

        if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_RETURN))
        {
          nameEntered = true;
          SDL_FreeSurface(message);
          message = TTF_RenderText_Solid(font, "Rank: 1st", textColor);
        }
      }

      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
    apply_surface(0, 0, background, screen);
    apply_surface((SCREEN_WIDTH - message->w) / 2, ((SCREEN_HEIGHT / 2) - message->h) / 2, message, screen);
    name.show_centered();


    if(SDL_Flip(screen) == -1)
    {
      return 1;
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

  SDL_WM_SetCaption("Scrolling Background", NULL);

  return true;
}

bool load_files()
{
  font = TTF_OpenFont("lazy.ttf", 42);
  background = load_image("bg.png");

  if(font == NULL)
  {
    return false;
  }

  if(background == NULL)
  {
    return false;
  }

  return true;
}

void clean_up()
{
  SDL_FreeSurface(message);
  SDL_FreeSurface(background);

  TTF_CloseFont(font);

  TTF_Quit();
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
      case SDLK_RIGHT: xVel += DOT_WIDTH / 2; break;
      case SDLK_LEFT: xVel -= DOT_WIDTH / 2; break;
      case SDLK_UP: yVel -= DOT_HEIGHT / 2; break;
      case SDLK_DOWN: yVel += DOT_HEIGHT / 2; break;
    }
  }
  else if(event.type == SDL_KEYUP)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_RIGHT: xVel -= DOT_WIDTH / 2; break;
      case SDLK_LEFT: xVel += DOT_WIDTH / 2; break;
      case SDLK_UP: yVel += DOT_HEIGHT / 2; break;
      case SDLK_DOWN: yVel -= DOT_HEIGHT / 2; break;
    }
  }
}

Dot::Dot()
{
  x = 0;
  y = 0;
  xVel = 0;
  yVel = 0;
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

void Dot::show()
{
//  apply_surface(x - camera.x, y - camera.y, dot, screen);
}

StringInput::StringInput()
{
  str = "";
  text = NULL;
  SDL_EnableUNICODE(SDL_ENABLE);
}

StringInput::~StringInput()
{
  SDL_FreeSurface(text);
  SDL_EnableUNICODE(SDL_DISABLE);
}

void StringInput::show_centered()
{
  if(text != NULL)
  {
    apply_surface((SCREEN_WIDTH - text->w) / 2, (SCREEN_HEIGHT - text->h) / 2, text, screen);
  }
}

void StringInput::handle_input()
{
  if(event.type == SDL_KEYDOWN)
  {
    std::string temp = str;

    if(str.length() <= 16)
    {
      if(event.key.keysym.unicode == (Uint16)' ')
      {
        str += (char)event.key.keysym.unicode;
      }
      else if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9'))
      {
        str += (char)event.key.keysym.unicode;
      }
      else if((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z'))
      {
        str += (char)event.key.keysym.unicode;
      }
      else if((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z'))
      {
        str += (char)event.key.keysym.unicode;
      }
    }

    if((event.key.keysym.sym == SDLK_BACKSPACE) && (str.length() != 0))
    {
      str.erase(str.length() - 1);
    }

    if(str != temp)
    {
      SDL_FreeSurface(text);
      text = TTF_RenderText_Solid(font, str.c_str(), textColor);
    }
  }
}
