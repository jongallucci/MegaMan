/**
 * MENU
 * A.D.S. Games
 * 06/05/2017
**/
#ifndef MENU_H
#define MENU_H
#define MAP_W 52
#define MAP_H 39
#define TILE_W 20
#define TILE_H 20

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "keyListener.h"
#include "joystickListener.h"
#include "tools.h"
#include "state.h"

enum _facing{
  LEFT, RIGHT
};

enum _action{
  STAND, JUMP, SHOOT, RUN, SLIDE
};

// Menu
class menu : public state{
  public:
    menu();
    virtual ~menu();

    void update();
    void draw();

  private:
      ALLEGRO_BITMAP* megaman[7];
      ALLEGRO_FONT* font;

      char facing;
      char action;
      char frame;
      int cutWidth;
      float x, y;
      float startX, startY;
      float xVelocity;
      float yVelocity;
      const float gravity = 0.75f;

      float mega_speed, mega_size;
      float width, height;
      ALLEGRO_TIMER* aniTimer;

      int tile_map[MAP_W][MAP_H];

      float quick_x[20];
      float quick_y[20];

      bool debug = false;      //test variables on/off
      bool canMoveLeft = true;
      bool canMoveRight = true;
      bool canMoveUp = true;
      bool canFall = true;
      bool smoothFall = false;
};

#endif // MENU_H
