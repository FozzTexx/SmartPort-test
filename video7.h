#include <stdint.h>

enum {
  GR_COLOR_BLACK = 0,
  GR_COLOR_MAGENTA,
  GR_COLOR_DARK_BLUE,
  GR_COLOR_PURPLE,
  GR_COLOR_DARK_GREEN,
  GR_COLOR_GREY1,
  GR_COLOR_MED_BLUE,
  GR_COLOR_LIGHT_BLUE,
  GR_COLOR_BROWN,
  GR_COLOR_ORANGE,
  GR_COLOR_GREY2,
  GR_COLOR_PINK,
  GR_COLOR_GREEN,
  GR_COLOR_YELLOW,
  GR_COLOR_AQUA,
  GR_COLOR_WHITE,
};

#define PAGE_TEXT	0
#define PAGE_COLOR	1

extern void v7_enable_color40(uint8_t foreground, uint8_t background);
extern void v7_set_backgroundxy(uint8_t background, uint8_t x, uint8_t y, uint8_t len);
extern void v7_selpage(uint8_t page);
extern void v7_movecolor(uint8_t dest_row, uint8_t src_row, uint8_t count);
extern void v7_set_colorxy(uint8_t foreground, uint8_t background,
			   uint8_t x, uint8_t y, uint8_t len);
