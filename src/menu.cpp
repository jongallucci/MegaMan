#include "menu.h"
#include "mouseListener.h"
#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <iostream>

// Init menu
menu::menu(){
  megaman[0] = al_load_bitmap("Run1.png");
  megaman[1] = al_load_bitmap("Run2.png");
  megaman[2] = al_load_bitmap("Run3.png");
  megaman[3] = al_load_bitmap("Stand.png");
  megaman[4] = al_load_bitmap("Jump.png");
  megaman[5] = al_load_bitmap("Stand_Shoot.png");
  megaman[6] = al_load_bitmap("Slide.png");
  aniTimer = al_create_timer(0.1);
  al_start_timer(aniTimer);

  x = 55;
  y = 55;
  startX = x;
  startY = y;
  facing = RIGHT;
  mega_speed = 4;
  mega_size = 2;
  cutWidth = 13;
  height = al_get_bitmap_height(megaman[3]);
  width = al_get_bitmap_width(megaman[2]) - cutWidth;

  //srand(time(NULL));

  // Map
  std::ifstream read("map.txt");
  for( int t = 0; t < MAP_H; t++)
    for (int i = 0; i < MAP_W; i++)
        read >> tile_map[i][t];
  read.close();

  font = al_load_font("consolas.ttf", 20, 0);

  for( int i = 19; i > 0; i--){
    quick_x[i] = 0.0f;
    quick_y[i] = 0.0f;
  }
}

// Destory menu
menu::~menu(){

}
// Update animation and wait for input
void menu::update(){
  xVelocity = 0;
  if( keyListener::key[ALLEGRO_KEY_A]){  // move left
    xVelocity = -mega_speed;
    facing = LEFT;
    if( !canFall)
      action = RUN;
  }
  if( keyListener::key[ALLEGRO_KEY_D]){ // move right
    xVelocity = mega_speed;
    facing = RIGHT;
    if( !canFall)
      action = RUN;
  }

  canMoveLeft = true;
  canMoveRight = true;
  canMoveUp = true;
  canFall = true;

  // Check for collision
  for (int i = 0; i < MAP_W; i++){
    for( int t = 0; t < MAP_H; t++){
      if(tile_map[i][t] == 1){
        // Left
        if( tools::collisionAny( x + xVelocity, x + 1.0f, i * TILE_W, (i + 1) * TILE_W,
                                 y + yVelocity, y + yVelocity + height * mega_size, t * TILE_H, (t + 1) * TILE_H)){
          if( tools::collisionLeft( x + xVelocity, x + 1.0f, (i + 1) * TILE_W)){
            canMoveLeft = false;
            xVelocity = 0.0f;
            x = ((i + 1) * TILE_W);
          }
        }

        // Right
        if( tools::collisionAny( x + width * mega_size - 1.0f, x + width * mega_size + xVelocity, i * TILE_W, (i + 1) * TILE_W,
                                 y + yVelocity, y + yVelocity + height * mega_size, t * TILE_H, (t + 1) * TILE_H)){
          if( tools::collisionRight( x + width * mega_size - 1.0f, x + width * mega_size + xVelocity, i * TILE_W)){
            canMoveRight = false;
            xVelocity = 0.0f;
            x = ((i * TILE_W) - (width * mega_size));
          }
        }
      }
    }
  }

  yVelocity += gravity;

  for (int i = 0; i < MAP_W; i++){
    for( int t = 0; t < MAP_H; t++){
      if(tile_map[i][t] == 1){
        // Rise
        if( yVelocity < 0 &&
            tools::collisionAny( x + xVelocity, x + xVelocity + width * mega_size, i * TILE_W, (i + 1) * TILE_W,
                                 y + yVelocity, y + 1.0f, t * TILE_H, (t + 1) * TILE_H)){
          if( tools::collisionTop( y + yVelocity, y + 1.0f, (t + 1) * TILE_H)){
            canMoveUp = false;
            yVelocity = 0.0f;
            y = ((t + 1) * TILE_H);
          }
        }

        // Fall
        if( yVelocity > 0 &&
            tools::collisionAny( x + xVelocity, x + xVelocity + width * mega_size, i * TILE_W, (i + 1) * TILE_W,
                                 y - 1.0f, y + yVelocity + height * mega_size, t * TILE_H, (t + 1) * TILE_H)){
          if( tools::collisionBottom( y - 1.0f, y + yVelocity + (height * mega_size), t * TILE_H)){ //HELP
            canFall = false;
            yVelocity = 0.0f;
            y = float(t * TILE_H) - (height * mega_size);
          }
        }
      }
    }
  }

  if( canFall){
    action = JUMP;
  }
  x += xVelocity;
  y += yVelocity;

  // changed after keyListeners
  // keyListeners for movement
  // change action and facing
  if( keyListener::anyKeyPressed != true && !canFall){    // stand
    action = STAND;
  }
  if( keyListener::key[ALLEGRO_KEY_W] && !canFall){       // jump
    yVelocity = -12.0f;
    action = JUMP;
  }
  if( keyListener::key[ALLEGRO_KEY_S]){                   // slide
    action = SLIDE;
  }

  // Left click adds blocks
  if( mouseListener::mouse_button & 1 || mouseListener::mouse_button & 2){
    int tileClickedX = (mouseListener::mouse_x) / TILE_W;
    int tileClickedY = (mouseListener::mouse_y) / TILE_H;

    if( tileClickedX >= 0 && tileClickedX < MAP_W &&
        tileClickedY >= 0 && tileClickedY < MAP_H){
      if( mouseListener::mouse_button & 1)
        tile_map[tileClickedX][tileClickedY] = 1;
      if( mouseListener::mouse_button & 2)
        tile_map[tileClickedX][tileClickedY] = 0;
    }

    std::ofstream saveMap;
    saveMap.open("map.txt");

    for( int i = 0; i < MAP_H; i++){
      for( int t = 0; t < MAP_W; t++){
        saveMap << tile_map[t][i] << " ";
        if(t == MAP_W - 1){
          saveMap << "\n";
        }
      }
    }
    saveMap.close();
  }

  if( keyListener::keyPressed[ALLEGRO_KEY_UP])
    mega_size++;
  if( keyListener::keyPressed[ALLEGRO_KEY_DOWN])
    mega_size--;
  if( keyListener::keyPressed[ALLEGRO_KEY_RIGHT])
    mega_speed++;
  if( keyListener::keyPressed[ALLEGRO_KEY_LEFT])
    mega_speed--;
  if(  keyListener::keyPressed[ALLEGRO_KEY_X]){
    x = mouseListener::mouse_x - ((width + cutWidth)/2 * mega_size);
    y = mouseListener::mouse_y - height/2 * mega_size;
  }
  if( keyListener::keyPressed[ALLEGRO_KEY_TILDE])
    debug = !debug;

  frame = al_get_timer_count(aniTimer)%3;

  if( debug){
    for( int i = 19; i > 0; i--){
      quick_x[i] = quick_x[i - 1];
      quick_y[i] = quick_y[i - 1];
    }
    quick_x[0] = x;
    quick_y[0] = y;
  }
}

// Draw images to screen
void menu::draw(){

  // Background
  al_clear_to_color( al_map_rgb(50,50,50));

  float jheight = al_get_bitmap_height(megaman[4]);
  if( debug){
    al_draw_filled_rectangle( x - (cutWidth/2) * mega_size, y, x + (width + cutWidth/2) * mega_size, y + height * mega_size, al_map_rgb(1, 255 , 0));
    al_draw_filled_rectangle( x, y, x + width * mega_size, y + height * mega_size, al_map_rgb(1, 113 , 239));

    for( int i = 0; i < 20; i++){
      al_draw_filled_rectangle( quick_x[i], quick_y[i], quick_x[i] + width * mega_size, quick_y[i] + height * mega_size, al_map_rgb(1, 113 - i * 5, 239 - i * 10));
    }

    // debug variable states in game
    al_draw_textf(font, al_map_rgb(255,255,255), 22,  20, 0, "canMoveLeft:  %d", canMoveLeft);
    al_draw_textf(font, al_map_rgb(255,255,255), 22,  40, 0, "canMoveRight: %d", canMoveRight);
    al_draw_textf(font, al_map_rgb(255,255,255), 22,  60, 0, "canMoveUp:    %d", canMoveUp);
    al_draw_textf(font, al_map_rgb(255,255,255), 22,  80, 0, "canFall:      %d", canFall);
    al_draw_textf(font, al_map_rgb(255,255,255), 22, 100, 0, "xVelocity:    %f", xVelocity);
    al_draw_textf(font, al_map_rgb(255,255,255), 22, 120, 0, "yVelocity:    %f", yVelocity);
    al_draw_textf(font, al_map_rgb(255,255,255), 22, 140, 0, "width:        %f", width*mega_size);
    al_draw_textf(font, al_map_rgb(255,255,255), 22, 160, 0, "y:            %f", y);
  }

  //reset height and width (SLIDE changes values)
  height = al_get_bitmap_height(megaman[3]);
  width = al_get_bitmap_width(megaman[2]) - cutWidth;

  int offset = 3;
  // Facing Right
  if( action == RUN && facing == RIGHT){
    al_draw_scaled_bitmap(megaman[frame], 0, 0, width + cutWidth, height, x - (cutWidth/2 - offset) * mega_size, y, (width + cutWidth) * mega_size, height * mega_size, 0);
  }
  else if( action == JUMP && facing == RIGHT){
    al_draw_scaled_bitmap(megaman[4], 0, 0, width + cutWidth, jheight, x - (cutWidth/2 - offset) * mega_size, y, (width + cutWidth) * mega_size, jheight * mega_size, 0);
  }
  else if( action == SHOOT && facing == RIGHT){
    al_draw_scaled_bitmap(megaman[5], 0, 0, width + cutWidth, height, x - (cutWidth/2 - offset) * mega_size, y, (width + cutWidth) * mega_size, height * mega_size, 0);
  }
  else if( action == STAND && facing == RIGHT){
    al_draw_scaled_bitmap(megaman[3], 0, 0, width + cutWidth, height, x - (cutWidth/2 - offset) * mega_size, y, (width + cutWidth) * mega_size, height * mega_size, 0);
  }
  else if( action == SLIDE && facing == RIGHT){
    height = al_get_bitmap_height(megaman[6]) - 1;
    width = al_get_bitmap_width(megaman[6]);
    al_draw_scaled_bitmap(megaman[6], 0, 0, width + cutWidth, height, x - (cutWidth/2 - offset) * mega_size, y + 1 * mega_size, (width + cutWidth) * mega_size, height * mega_size, 0);
  }
  // Facing Left
  if( action == RUN && facing == LEFT){
    al_draw_scaled_bitmap(megaman[frame], 0, 0, width + cutWidth, height, x - (cutWidth/2 + offset) * mega_size, y, (width + cutWidth) * mega_size, height * mega_size, 1);
  }
  else if( action == JUMP && facing == LEFT){
    al_draw_scaled_bitmap(megaman[4], 0, 0, width + cutWidth, jheight, x - (cutWidth/2 + offset) * mega_size, y, (width + cutWidth) * mega_size, jheight * mega_size, 1);
  }
  else if( action == SHOOT && facing == LEFT){
    al_draw_scaled_bitmap(megaman[5], 0, 0, width + cutWidth, height, x - (cutWidth/2 + offset) * mega_size, y, (width + cutWidth) * mega_size, height * mega_size, 1);
  }
  else if( action == STAND && facing == LEFT){
    al_draw_scaled_bitmap(megaman[3], 0, 0, width + cutWidth, height, x - (cutWidth/2 + offset) * mega_size, y, (width + cutWidth) * mega_size, height * mega_size, 1);
  }
  else if( action == SLIDE && facing == LEFT){
    height = al_get_bitmap_height(megaman[6]) - 1;
    width = al_get_bitmap_width(megaman[6]);
    al_draw_scaled_bitmap(megaman[6], 0, 0, width + cutWidth, height, x - (cutWidth/2 + offset) * mega_size, y + 1 * mega_size, (width + cutWidth) * mega_size, height * mega_size, 1);
  }

  // Draw Map
  for( int i = 0; i < MAP_W; i++){
    for( int j = 0; j < MAP_H; j++){
      if( tile_map[i][j] == 1){
          al_draw_filled_rectangle( i * TILE_W, j * TILE_H, (i + 1) * TILE_W, (j + 1) * TILE_H, al_map_rgb(255, 20 , 120));
      }
    }
  }
}
