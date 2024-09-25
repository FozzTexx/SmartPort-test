#include "appleid.h"
#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <peekpoke.h>

#define WINDOW_LEFT	32
#define WINDOW_WIDTH	33
#define WINDOW_TOP	34
#define WINDOW_BOTTOM	35
#define CURSOR_PTR	40

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

void update_tries(int tries, int fails)
{
  int x, y, top;


  x = wherex();
  y = wherey();
  top = PEEK(WINDOW_TOP);
  POKE(WINDOW_TOP, 0);
  gotoxy(LOC_TRIES_X, LOC_TRIES_Y);
  printf("Tries: %-5i  Fails: %-5i", tries, fails);
  POKE(WINDOW_TOP, top);
  gotoxy(x, y);
  return;
}

void show_devices(const char *const *devices, const uint8_t *devstat, int count)
{
  int idx, len, top;
  uint8_t *ptr;
  static uint8_t longest = 0, column = 0, alt = 0;
  static int last_count = 0;


  if (last_count < count) {
    for (idx = 0; idx < count; idx++) {
      len = strlen(devices[idx]);
      if (len > longest)
	longest = len;
    }
  }

  if (column < longest + 1)
    column = longest;
  column += 1;
  if (column >= 40) {
    top = PEEK(WINDOW_TOP);
    for (idx = 0; idx < last_count; idx++) {
      ptr = (uint8_t *) (0x400 + (top + idx) / 8 * 0x28 +
			 0x80 * ((top + idx) % 8) + longest + 1);
      memmove(ptr, ptr+1, 39 - longest - 1);
    }
    column = 39;
  }

  for (idx = 0; idx < count; idx++) {
    if (last_count < count) {
      gotoxy(0, idx);
      printf("%-*s", longest, devices[idx]);
    }
    gotoxy(column, idx);
    cputc(devstat[idx] ? (alt ? ',' : '.') : 'X');
  }

  gotoxy(0, idx);
  last_count = count;
  alt = (alt + 1) % 2;

  return;
}
