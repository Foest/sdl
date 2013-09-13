#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

//Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//Globals
SDL_Surface *screen = NULL;
SDL_Surface *bitmapFont= NULL;
SDL_Event event;

//Structs/Classes
class BitmapFont
{
  private:
    SDL_Surface *bitmap;
    SDL_Rect chars[256];
    int newLine, space;

  public:
    BitmapFont();
    BitmapFont(SDL_Surface *surface);
    void build_font(SDL_Surface *surface);
    void show_text(int x, int y, std::string text, SDL_Surface *surface);
};

//Prototypes
bool init();
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL);
SDL_Surface *load_image(std::string filename);
bool load_files();
void clean_up();
bool check_collision(SDL_Rect A, SDL_Rect B);
Uint32 get_pixel32(int x, int y, SDL_Surface * surface);

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

  BitmapFont font(bitmapFont);
  SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
  font.show_text(100, 100, "Bitmap Font:\nABCDEFGHIJKLMNOPQRSTUVWXYZ
      \nabcdefghijklmnopqrstuvwxyz
      \n-123456789", screen);

    if(SDL_Flip(screen) == -1)
    {
      return 1;
    }

  //While user hasn't quit
  while(quit == false)
  {
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
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

  SDL_WM_SetCaption("Bitmap Font Test", NULL);

  srand(SDL_GetTicks());

  return true;
}

bool load_files()
{
  bitmapFont = load_image("lazyfont.png");

  if(bitmapFont == NULL)
  {
    return false;
  }

  return true;
}

void clean_up()
{
  SDL_FreeSurface(bitmapFont);
  SDL_Quit();
}

bool check_collision(SDL_Rect A, SDL_Rect B)
{
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;
  leftA = A.x;
  rightA = A.x + A.w;
  topA = A.y;
  bottomA = A.y + A.h;
  leftB = B.x;
  topB = B.y;
  rightB = B.x + B.w;
  bottomB = B.y + B.h;

  if(bottomA <= topB)
  {
    return false;
  }

  if(topA >= bottomB)
  {
    return false;
  }

  if(rightA <= leftB)
  {
    return false;
  }

  if(leftA >= rightB)
  {
    return false;
  }
  return true;
}

Uint32 get_pixel32(int x, int y, SDL_Surface *surface)
{
  Uint32 *pixels = (Uint32 *)surface->pixels;
  return pixels[(y * surface->w) + x];
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
  box.x = 0;
  box.y = 0;
  box.w = DOT_WIDTH;
  box.h = DOT_HEIGHT;
  xVel = 0;
  yVel = 0;
}

void Dot::set_camera()
{
  camera.x = (box.x + DOT_WIDTH / 2) - SCREEN_WIDTH/2;
  camera.y = (box.y + DOT_HEIGHT/ 2) - SCREEN_HEIGHT/2;

  if(camera.x < 0)
  {
    camera.x = 0;
  }

  if(camera.y < 0)
  {
    camera.y = 0;
  }

  if(camera.x > LEVEL_WIDTH - camera.w)
  {
    camera.x = LEVEL_WIDTH - camera.w;
  }

  if(camera.y > LEVEL_HEIGHT - camera.h)
  {
    camera.y = LEVEL_HEIGHT - camera.h;
  }
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

void Dot::show()
{
  apply_surface(box.x - camera.x, box.y - camera.y, dot, screen);
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

BitmapFont::()
{
  bitmap = NULL;
  newLine = 0;
  space = 0;
}

BitmapFont::(SDL_Surface *surface)
{
  build_font(surface);
}

void BitmapFont::show_text(int x, int y, std::string text, SDL_Surface *surface)
{
  int X = x, Y = y;

  if(bitmap != NULL)
  {
    for (int show = 0; text[show] != '\0'; show++)
    {
      if(text[show] == ' ')
      {
        X += space;
      }
      else if(text[show] == '\n')
      {
        Y += newLine;
        X = x;
      }
      else
      {
        int ascii = (unsigned char) text[show];
        apply_surface(X, Y, bitmap, surface, &chars[ascii]);
        X += chars[ascii].w + 1;
      }
    }
  }
}
