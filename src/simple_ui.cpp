#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdio.h>
#include "io.h"
#define PIXELSIZE 10

using namespace sf;

static Color gray(0xBB, 0xBB, 0xBB);
static RectangleShape* grid[11][128];
static int shifts[11] = {3, 2, 1, 9, 8, 15, 14, 13, 12, 11, 10};

uint16_t column_to_led(int j){
  uint16_t neutral = 0xFFFF;
  int i;
  for(i = 0; i < 11; i++){
    if(grid[i][j]->getFillColor() == gray)
      continue;
    uint16_t mask = ~(1 << shifts[i]);
    neutral &= mask;
  }
  return neutral;
}

void button_clicked(hid_device* handle, Event::MouseButtonEvent mouseevt){
  if(mouseevt.y >= 12*PIXELSIZE &&
     mouseevt.x <  10*PIXELSIZE){

    uint8_t buffer[2058];
    read_rom_from_usb(handle, buffer, 2058, NULL);
   
    int wr = 0x10;
    buffer[wr++] = 1; // Number patterns
    buffer[wr++] = 0x80; // Number colons
    buffer[wr++] = 0x91;
    buffer[wr++] = 0;
    buffer[wr++] = 0x15;

    int i;
    for(i = 127; i >=0; i--){
      uint16_t res = column_to_led(i);
      buffer[wr++] = res >> 8;
      buffer[wr++] = res & 0xFF;
    } 
    
    write_rom_to_usb(handle, buffer, 2058, NULL);
    printf("Finished !\n");
  }
}

void analyse_button(hid_device* handle,
		    int* changed,
		    Event::MouseButtonEvent mouseevt){
  if(mouseevt.y >= 11*PIXELSIZE){
    button_clicked(handle, mouseevt);
    return;
  }

  int i = mouseevt.y / PIXELSIZE;
  int j = mouseevt.x / PIXELSIZE;
  
  RectangleShape* toedit = grid[i][j];
  if(mouseevt.button == Mouse::Right){
    toedit->setFillColor(Color(0xBB, 0xBB, 0xBB));
  } else {
    toedit->setFillColor(Color(0, 0, 0));
  }
  *changed = 1;
}

void main_loop(hid_device* handle){
  RenderWindow window(VideoMode(128*PIXELSIZE, (11+5)*PIXELSIZE), "LED Editor");

  window.setFramerateLimit(60);
  
  int i, j, changed = 1;

  for(i = 0; i < 11; i++){
    for(j = 0; j < 128; j++){
      RectangleShape* newrect =
	new RectangleShape(Vector2f(PIXELSIZE, PIXELSIZE));
      newrect->setPosition(Vector2f(j*PIXELSIZE, i*PIXELSIZE));
      newrect->setFillColor(gray);
      newrect->setOutlineColor(Color::Black);
      newrect->setOutlineThickness(1);
      grid[i][j] = newrect;
    }
  }

  RectangleShape save(Vector2f(10*PIXELSIZE, 4*PIXELSIZE));
  save.setPosition(Vector2f(0, 12*PIXELSIZE));
  save.setFillColor(Color(0, 153, 230));

  while(window.isOpen()){
    Event event;
    while(window.pollEvent(event)){
      if(event.type == Event::Closed) {
	window.close();
      } else if(event.type == Event::MouseButtonPressed) {
	analyse_button(handle, &changed, event.mouseButton);
      }
    }
    
    window.clear(gray);
    
    for(i = 0; i < 11; i++)
      for(j = 0; j < 128; j++)
	window.draw(*grid[i][j]);

    window.draw(save);
    
    window.display();
  }
}
