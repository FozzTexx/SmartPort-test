#include <apple2.h>
#include <stdio.h>
#include <conio.h>
#include <stdint.h>

typedef uint8_t *address;

#include "callsp.h"

typedef struct {
  uint8_t count;
  uint8_t unit;
} SmartPort_Parameters;

address SmartPort_MLI;

address find_smartport()
{
  int slot;
  address smartport = 0, ptr;


  for (slot = 7; slot > 0; slot--) {
    ptr = (uint8_t *) (0xc000 + slot * 256);
    if (*(ptr + 1) == 0x20 &&
	*(ptr + 3) == 0x00 &&
	*(ptr + 5) == 0x03 &&
	*(ptr + 7) == 0x00) {
      printf("Found SmartPort %i\n", slot);
      smartport = ptr + *(ptr + 0xff) + 3;
      break;
    }
  }

  return smartport;
}

void main()
{
  SmartPort_Parameters parms;
  uint8_t err, rcv;
  uint16_t count;


  printf("Hello\n");
  SmartPort_MLI = find_smartport();
  printf("SmartPort: %04x\n", SmartPort_MLI);

  parms.count = 1;
  parms.unit = 0;

  printf("%04x %x %04x\n", SmartPort_MLI, 5, &parms);
  for (err = count = 0; !err; count++) {
    if (kbhit()) {
      rcv = cgetc();
      if (rcv == 27)
	break;
    }
    
    err = callsp(SmartPort_MLI, 5, (address) &parms);
    printf("Count: %i  Error: %02x\n", count, err);
  }

  return;
}
