#include "appleid.h"
#include <conio.h>
#include <stdio.h>
#include <peekpoke.h>

#define WINDOW_LEFT	32
#define WINDOW_WIDTH	33
#define WINDOW_TOP	34
#define WINDOW_BOTTOM	35

#define LOC_TRIES_X	0
#define LOC_TRIES_Y	4

void ui_init(const char *fn_version)
{
  int x, y;


  // Reset text window and clear screen
  POKE(WINDOW_LEFT, 0);
  POKE(WINDOW_WIDTH, 40);
  POKE(WINDOW_TOP, 0);
  POKE(WINDOW_BOTTOM, 24);
  clrscr();

  gotoxy(0, 0);
  printf("Machine: %s\n", apple_name(get_ostype()));
  printf("FujiNet: %s\n", fn_version);
  printf("FN Board: ???\n");
  printf("Time: ???\n");
  printf("Tries: 0\n");

  y = wherey() + 1;
  POKE(34, y);
  gotoxy(0, y);
  return;
}

void update_tries(int tries)
{
  int x, y, top;


  x = wherex();
  y = wherey();
  top = PEEK(WINDOW_TOP);
  POKE(WINDOW_TOP, 0);
  gotoxy(LOC_TRIES_X, LOC_TRIES_Y);
  printf("Tries: %i", tries);
  POKE(WINDOW_TOP, top);
  gotoxy(x, y);
  return;
}
