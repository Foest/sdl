#include "iostream"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <sstream>
#include <string>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;
const int DOT_HEIGHT = 20;
const int DOT_WIDTH = 20;

SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;

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

  SDL_WM_SetCaption("Collision Detection", NULL);

  return true;
}

bool load_files()
{
  //Load image
  square = load_image("square.bmp");

  //Open the font
  font = TTF_OpenFont("lazy.ttf", 30);

  //If there was an error loading the images
  if(square == NULL)
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
  SDL_FreeSurface(square);

  TTF_CloseFont(font);

  TTF_Quit();
  SDL_Quit();
}

bool check_collision(std::vector<SDL_Rect> &A, std::vector<SDL_Rect> &B)
{
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;

  for(int Abox = 0; Abox < A.size(); Abox++)
  {
    leftA = A[Abox].x;
    rightA = A[Abox].x + A[Abox].w;
    topA = A[Abox].y;
    bottomA = A[Abox].y + A[Abox].h;

    for(int Bbox = 0; Bbox < B.size(); Bbox++)
    {
      leftB = B[Bbox].x;
      rightB = B[Bbox].x + B[Bbox].w;
      topB = B[Bbox].y;
      bottomB = B[Bbox].y + B[Bbox].h;

      if(((bottomA <= topB) || (topA >= bottomB) || (rightA <= leftB) || (leftA > = rightB)) == false)
      {
        return true;
      }
    }
  }
  return false;
}

class Square
{
  private:
    SDL_Rect box;
    int xVel, yVel;
  public:
    Square();
    void handle_input();
    void move();
    void show();
};

Square::Square()
{
  box.x = 0;
  box.y = 0;

  box.w = SQUARE_WIDTH;
  box.h = SQUARE_HEIGHT;

  xVel = 0;
  yVel = 0;
}

void Square::move()
{
  box.x += xVel;

  if((box.x < 0) || (box.x + SQUARE_WIDTH > SCREEN_WIDTH) || (check_collision(box, wall)))
  {
    box.x -= xVel;
  }

  box.y += yVel;
  if((box.y < 0) || (box.y + SQUARE_HEIGHT > SCREEN_HEIGHT) || (check_collision(box, wall)))
  {
    box.y -= yVel;
  }
}

void Square::handle_input()
{
  if(event.type == SDL_KEYDOWN)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_UP: yVel -= SQUARE_HEIGHT / 2; break;
      case SDLK_DOWN: yVel += SQUARE_HEIGHT / 2; break;
      case SDLK_LEFT: xVel -= SQUARE_WIDTH / 2; break;
      case SDLK_RIGHT: xVel += SQUARE_WIDTH / 2; break;
    }
  }
  else if(event.type == SDL_KEYUP)
  {
    switch(event.key.keysym.sym)
    {
      case SDLK_UP: yVel += SQUARE_HEIGHT / 2; break;
      case SDLK_DOWN: yVel -= SQUARE_HEIGHT / 2; break;
      case SDLK_LEFT: xVel += SQUARE_WIDTH / 2; break;
      case SDLK_RIGHT: xVel -= SQUARE_WIDTH / 2; break;
    }
  }
}

void Square::show()
{
  apply_surface(box.x, box.y, square, screen);
}

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

class Dot
{
  private:
    int x, y;
    std::vector<SDL_Rect> box;
    int xVel, yVel;
    void shift_boxes()

  public:
      Dot(int X, int Y);
      void handle_input();
      void move(std::vector<SDL_Rect> &rects);
      void show();
      std::vector<SDL_Rect> &get_rects();
};

Dot::Dot(int X, int Y)
{
  x = X;
  y = Y;
  xVel = 0;
  yVel = 0;
  box.resize(11);

  box[0].w = 6;
  box[0].h = 1;
  box[1].w = 10;
  box[1].h = 1;
  box[2].w = 14;
  box[2].h = 1;
  box[3].w = 16;
  box[3].h = 2;
  box[4].w = 18;
  box[4].h = 2;
  box[5].w = 20;
  box[5].h = 6;
  box[6].w = 18;
  box[6].h = 2;
  box[7].w = 16;
  box[7].h = 2;
  box[8].w = 14;
  box[8].h = 1;
  box[9].w = 10;
  box[9].h = 1;
  box[10].w = 6;
  box[10].h = 1;

  shift_boxes();
}

void Dot::shift_boxes()
{
  int r = 0;

  for(int set = 0; set < box.size(); set++)
  {
    box[set].x = x + (DOT_WIDTH - box[set].w) / 2;
    box[set].y = y + r;
    r += box[set].h;
  }
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


int main(int argc, char* args[])
{
  bool quit = false;
  bool cap = true;
  int frame = 0;
  Timer fps;
  Square mySquare;

  if(init() == false)
  {
    return 1;
  }

  //Load the files
  if(load_files() == false)
  {
    return 1;
  }

  wall.x = 300;
  wall.y = 40;
  wall.w = 40;
  wall.h = 400;

  //While user hasn't quit
  while(quit == false)
  {
    fps.start();

    while(SDL_PollEvent(&event))
    {
      mySquare.handle_input();

      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
    }
      mySquare.move();
      SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
      SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 0x77, 0x77, 0x77));
      mySquare.show();

      if(SDL_Flip(screen) == -1)
      {
        return 1;
      }

      frame++;

      if(fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
      {
        SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
      }
  }

  clean_up();
  return 0;
}
