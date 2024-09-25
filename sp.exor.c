#include <apple2.h>
#include <stdio.h>
#include <conio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t *address;

#include "callsp.h"
#include "interface.h"

enum {
  SP_CMD_STATUS         = 0x00,
  SP_CMD_READBLOCK      = 0x01,
  SP_CMD_WRITEBLOCK     = 0x02,
  SP_CMD_FORMAT         = 0x03,
  SP_CMD_CONTROL        = 0x04,
  SP_CMD_INIT           = 0x05,
  SP_CMD_OPEN           = 0x06,
  SP_CMD_CLOSE          = 0x07,
  SP_CMD_READ           = 0x08,
  SP_CMD_WRITE          = 0x09,
};

typedef struct {
  uint8_t count;
  uint8_t unit;
  address status;
  uint8_t type;
} SmartPort_Parameters;

address SmartPort_MLI;
char buffer[32];

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
  uint16_t tries;
  int idx, dev_count, found_fuji, command;


  ui_init();

  SmartPort_MLI = find_smartport();

  for (err = tries = 0; !err; tries++) {
    if (kbhit()) {
      rcv = cgetc();
      if (rcv == 27)
        break;
    }

    update_tries(tries);

    parms.count = 1;
    parms.unit = 0;
    command = SP_CMD_INIT;
    err = callsp(SmartPort_MLI, command, (address) &parms);
    if (err)
      goto done;

    parms.count = 3;
    parms.unit = 0;
    parms.status = (address) buffer;
    parms.type = 0;

    command = SP_CMD_STATUS;
    err = callsp(SmartPort_MLI, command, (address) &parms);
    if (err)
      goto done;

    dev_count = buffer[0];
    printf("  Num devices: %i\n", dev_count);
    if (!dev_count)
      break;

    // Walk all devices to find FujiNet
    for (found_fuji = 0, idx = 1; idx <= dev_count; idx++) {
      parms.count = 3;
      parms.unit = idx;
      parms.status = (address) buffer;
      parms.type = 3;

      command = SP_CMD_STATUS;
      err = callsp(SmartPort_MLI, command, (address) &parms);
      if (err)
	goto done;

      if (!found_fuji && !strncmp(&buffer[5], "FUJINET", 7))
	found_fuji = 1;
      printf("    %i: %.*s\n", idx, buffer[4], &buffer[5]);
    }

    if (!found_fuji) {
      printf("No FujiNet\n");
      break;
    }

  done:
    printf("Cmd: %i  Unit: %i  Error: %02x\n", command, parms.unit, err);
  }

  return;
}
