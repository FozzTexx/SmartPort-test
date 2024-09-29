#include <apple2.h>
#include <stdio.h>
#include <conio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fujinet-fuji.h>

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
AdapterConfigExtended ace;
SmartPort_Parameters parms;

address find_smartport()
{
  int slot;
  address ptr;


  for (slot = 7; slot > 0; slot--) {
    ptr = (uint8_t *) (0xc000 + slot * 256);
    if (*(ptr + 1) == 0x20 &&
        *(ptr + 3) == 0x00 &&
        *(ptr + 5) == 0x03 &&
        *(ptr + 7) == 0x00) {
      //printf("Found SmartPort %i\n", slot);
      return ptr + *(ptr + 0xff) + 3;
    }
  }

  return 0;
}

void main()
{
  uint8_t err, rcv;
  uint16_t tries;
  int idx, dev_count, found_fuji, command;
  uint8_t instafail = 0;
  char **devices = NULL;
  uint8_t *devstat = NULL;
  int numdev = 0;


  if (!fuji_get_adapter_config_extended(&ace)) {
    strcpy(ace.fn_version, "FAIL");
    instafail = 1;
  }
  ui_init(ace.fn_version);
#if 0
  if (instafail)
    return;
#endif

  SmartPort_MLI = find_smartport();
  if (!SmartPort_MLI) {
    printf("No SmartPort controller found\n");
    return;
  }

  for (err = tries = 0; /*!err*/; tries++) {
    if (kbhit()) {
      rcv = cgetc();
      if (rcv == 27)
        break;
    }

    //clrscr();
    update_tries(tries);

    parms.count = 1;
    parms.unit = 0;
    command = SP_CMD_INIT;
    //printf("I %04x ", SmartPort_MLI);
    err = callsp(SmartPort_MLI, command, (address) &parms);
    if (err) {
      log_error("SmartPort INIT failed err %02x", err);
      continue;
    }

    //cputc('i');
    parms.count = 3;
    parms.unit = 0;
    parms.status = (address) buffer;
    parms.type = 0;

    command = SP_CMD_STATUS;
    //cputc('D');
    err = callsp(SmartPort_MLI, command, (address) &parms);
    if (err) {
      log_error("Unable to fetch device count err %02x", err);
      continue;
    }

    dev_count = buffer[0];
    if (dev_count < numdev)
      log_error("Device list incomplete");

    if (dev_count && (!devices || numdev < dev_count)) {
      devices = realloc(devices, sizeof(char *) * dev_count);
      devstat = realloc(devstat, dev_count);
      memset(&devices[numdev], 0, sizeof(char *) * (dev_count - numdev));
      numdev = dev_count;
    }

    memset(devstat, 0, numdev);

    // Walk all devices to find FujiNet
    for (found_fuji = 0, idx = 0; idx < dev_count; idx++) {
      parms.count = 3;
      parms.unit = idx + 1;
      parms.status = (address) buffer;
      parms.type = 3;

      command = SP_CMD_STATUS;
      err = callsp(SmartPort_MLI, command, (address) &parms);
      if (!err)
	devstat[idx] = 1;
      else
	log_error("Unit %i status failed err %02x", idx + 1, err);

      if (!devices[idx]) {
	devices[idx] = malloc(buffer[4] + 1);
	strncpy(devices[idx], &buffer[5], buffer[4]);
	devices[idx][buffer[4]] = 0;
      }

      if (!found_fuji && !strncmp(&buffer[5], "FUJINET", 7))
	found_fuji = 1;
    }

    show_devices((const char *const *) devices, devstat, numdev);

    if (!found_fuji || instafail) {
      log_error("FujiNet not found");
      instafail = 0;
    }
    else
      update_time();
  }

  return;
}
