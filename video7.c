#include "video7.h"

#include <apple2enh.h>
#include <peekpoke.h>
#include <conio.h>
#include <string.h>

#define SWITCH_80STORE_SET	0xC001
#define SWITCH_80STORE_RESET	0xC000
#define SWITCH_PG2_SET		0xC055
#define SWITCH_PG2_RESET	0xC054
#define SWITCH_AN3_SET		0xC05F
#define SWITCH_AN3_RESET	0xC05E

#define TEXT_PAGE_1		0x400
#define SCREEN_SIZE		0x400

#define TEXTADDR_XY(x, y) ((TEXT_PAGE_1 + (y) / 8 * 0x28 + 0x80 * ((y) % 8) + x))

void v7_enable_color40(uint8_t foreground, uint8_t background)
{
  uint8_t combi;

  videomode(VIDEOMODE_40COL);
  POKE(SWITCH_80STORE_SET, 0);
  POKE(SWITCH_AN3_RESET, 0);

  combi = (foreground << 4) | (background & 0xf);
  POKE(SWITCH_PG2_SET, 0);
  memset((uint8_t *) TEXT_PAGE_1, combi, SCREEN_SIZE);
  POKE(SWITCH_PG2_RESET, 0);
  return;
}

void v7_set_backgroundxy(uint8_t background, uint8_t x, uint8_t y, uint8_t len)
{
  uint8_t cur;
  uint16_t addr;
  uint8_t idx;


  addr = TEXTADDR_XY(x, y);
  POKE(SWITCH_PG2_SET, 0);
  for (idx = 0; idx < len; idx++, addr++) {
    cur = PEEK(addr);
    cur = (cur & 0xf0) | (background & 0xf);
    POKE(addr, cur);
  }
  POKE(SWITCH_PG2_RESET, 0);

  return;
}

void v7_set_colorxy(uint8_t foreground, uint8_t background, uint8_t x, uint8_t y, uint8_t len)
{
  uint8_t combi;
  uint16_t addr;


  combi = (foreground << 4) | (background & 0xf);
  addr = TEXTADDR_XY(x, y);
  POKE(SWITCH_PG2_SET, 0);
  memset((uint8_t *) addr, combi, len);
  POKE(SWITCH_PG2_RESET, 0);

  return;
}

void v7_selpage(uint8_t page)
{
  if (page)
    POKE(SWITCH_PG2_SET, 0);
  else
    POKE(SWITCH_PG2_RESET, 0);
  return;
}

void v7_movecolor(uint8_t dest_row, uint8_t src_row, uint8_t count)
{
  uint8_t idx;
  uint8_t *src, *dest;


  POKE(SWITCH_PG2_SET, 0);
  for (idx = 0; idx < count; idx++) {
    dest = (uint8_t *) TEXTADDR_XY(0, dest_row + idx);
    src = (uint8_t *) TEXTADDR_XY(0, src_row + idx);
    memmove(dest, src, 40);
  }
  POKE(SWITCH_PG2_RESET, 0);

  return;
}
