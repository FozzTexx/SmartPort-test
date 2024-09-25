#include "appleid.h"
#include <stdint.h>
#include <apple2.h>

struct apple_name {
  uint8_t id;
  const char *name;
};

struct apple_name apple_map[] = {
  {APPLE_UNKNOWN, "Unknown Apple ]["},
  {APPLE_II, "Apple ]["},
  {APPLE_IIPLUS, "Apple ][ Plus"},
  {APPLE_IIJPLUS, "Apple ][ J-Plus"},
  {APPLE_IIIEM, "Apple ///"},
  {APPLE_IIE, "Apple IIe"},
  {APPLE_IIEENH, "Apple IIe (enhanced)"},
  {APPLE_IIECARD, "Apple IIe option card"},
  {APPLE_IIC, "Apple IIc"},
  {APPLE_IIC35, "Apple IIc ROM 0"},
  {APPLE_IICEXP, "Apple IIc ROM 3"},
  {APPLE_IICREV, "Apple IIc ROM 4"},
  {APPLE_IICPLUS, "Apple IIc Plus"},
  {APPLE_IIGS, "Apple IIgs"},
  {APPLE_IIGS1, "Apple IIgs ROM 1"},
  {APPLE_IIGS3, "Apple IIgs ROM 3"},
  {0x00, NULL},
};

const char *apple_name(int apple_id)
{
  int idx;


  for (idx = 0; apple_map[idx].name; idx++)
    if (apple_id == apple_map[idx].id)
      return apple_map[idx].name;

  return apple_map[0].name;
}
