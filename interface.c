#include "appleid.h"
#include "video7.h"

#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <peekpoke.h>
#include <fujinet-clock.h>

#define WINDOW_LEFT	32
#define WINDOW_WIDTH	33
#define WINDOW_TOP	34
#define WINDOW_BOTTOM	35
#define CURSOR_PTR	40

uint8_t tries_row, time_row;
uint8_t device_count;
uint16_t err_count = 0;
uint8_t is_color = 0;

void ui_init(const char *fn_version)
{
  uint8_t machine_type;


  // Reset text window and clear screen
  POKE(WINDOW_LEFT, 0);
  POKE(WINDOW_WIDTH, 40);
  POKE(WINDOW_TOP, 0);
  POKE(WINDOW_BOTTOM, 24);
  clrscr();

  machine_type = get_ostype();
  if (machine_type == APPLE_IIE || machine_type == APPLE_IIEENH) {
    v7_enable_color40(GR_COLOR_WHITE, GR_COLOR_MED_BLUE);
    is_color = 1;
  }

  gotoxy(0, 0);
  printf("Machine: %s\n", apple_name(machine_type));
  printf("FujiNet: %s\n", fn_version);
  printf("FN Board: ???\n");

  time_row = wherey();
  printf("Time: ???\n");

  tries_row = wherey();
  printf("Tries: 0\n");

  POKE(WINDOW_TOP, tries_row + 1);
  return;
}

void update_xy(uint8_t x, uint8_t y, const char *format, ...)
{
  uint8_t cx, cy, top;
  va_list ap;


  top = PEEK(WINDOW_TOP);
  cx = wherex();
  cy = wherey();
  POKE(WINDOW_TOP, 0);
  gotoxy(x, y);
  va_start(ap, format);
  vcprintf(format, ap);
  va_end(ap);
  POKE(WINDOW_TOP, top);
  gotoxy(cx, cy);
  return;
}

void update_tries(int tries)
{
  update_xy(0, tries_row, "Tries: %-5i  Errors: %-5i", tries, err_count);
  return;
}

void update_time()
{
  uint8_t tbuf[7];
  struct datetime pbuf;
  time_t now, runtime;
  static time_t start = 0;


  if (clock_get_time(tbuf, SIMPLE_BINARY) == FN_ERR_OK) {
    pbuf.date.year = tbuf[1];
    pbuf.date.mon = tbuf[2];
    pbuf.date.day = tbuf[3];
    pbuf.time.hour = tbuf[4];
    pbuf.time.min = tbuf[5];
    now = mktime_dt(&pbuf) + tbuf[6];
    if (!start)
      start = now;
    runtime = now - start;
    update_xy(0, time_row, "Time: %i:%02i:%02i   Run: %i:%02i:%02i ",
	      tbuf[4], tbuf[5], tbuf[6],
	      (int) runtime / 3600, (int) (runtime % 3600) / 60, (int) runtime % 60);
  }

  return;
}

void show_devices(const char *const *devices, const uint8_t *devstat, uint8_t count)
{
  uint8_t idx, len, top;
  uint8_t *ptr;
  uint8_t x, y;
  static uint8_t longest = 0, column = 0, alt = 0;


  x = wherex();
  y = wherey() + PEEK(WINDOW_TOP);
  POKE(WINDOW_TOP, 0);

  top = tries_row + 2;

  if (device_count < count) {
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
    for (idx = 0; idx < device_count; idx++) {
      ptr = (uint8_t *) (0x400 + (top + idx) / 8 * 0x28 +
			 0x80 * ((top + idx) % 8) + longest + 1);
      memmove(ptr, ptr+1, 39 - longest - 1);
      if (is_color) {
	v7_selpage(PAGE_COLOR);
	memmove(ptr, ptr+1, 39 - longest - 1);
	v7_selpage(PAGE_TEXT);
      }
    }
    column = 39;
  }

  for (idx = 0; idx < count; idx++) {
    if (device_count < count) {
      gotoxy(0, idx + top);
      printf("%-40s", devices[idx]);
    }
    cputcxy(column, idx + top, devstat[idx] ? (alt ? ',' : '.') : 'X');
    if (is_color)
      v7_set_backgroundxy(devstat[idx] ? GR_COLOR_GREEN : GR_COLOR_ORANGE,
			  column, idx + top, 1);
  }

  device_count = count;
  alt = (alt + 1) % 2;

  top = tries_row + device_count + 3;
  if (y < top)
    y = top;
  POKE(WINDOW_TOP, top);
  gotoxy(x, y - top);

  return;
}

void log_error(const char *format, ...)
{
  va_list ap;
  uint8_t y1, y2, top;


  err_count++;
  va_start(ap, format);
  if (is_color)
    y1 = wherey();
  printf("\n");
  if (is_color) {
    top = PEEK(WINDOW_TOP);
    y2 = wherey();

    if (y1 == y2) {
      // Must have scrolled, scroll the color too
      v7_selpage(PAGE_COLOR);
      printf("\n");
      v7_selpage(PAGE_TEXT);
    }
  }
  printf("%u: ", err_count);
  if (is_color) {
    v7_set_colorxy(GR_COLOR_WHITE, GR_COLOR_ORANGE, 0, y2 + top, 40);
  }
  vcprintf(format, ap);
  va_end(ap);
  return;
}
