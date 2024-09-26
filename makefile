PLATFORM=apple2enh
CC=cl65
FUJINET_LIB_DIR=$(HOME)/source/fujinet/fujinet-lib
CFLAGS=-O -I$(FUJINET_LIB_DIR)
LIBS=-L $(FUJINET_LIB_DIR)/build fujinet.lib

define single-to-double
  unsingle $< && mv $<.ad $@ && mv .AppleDouble/$<.ad .AppleDouble/$@
endef

define link-bin
  $(CC) -m $@.map -vm -t $(PLATFORM) $(LDFLAGS) $^ $(LIBS) -o $@
endef

all: sp.exor

sp.exor: sp.exor.bin
	$(single-to-double)
sp.exor.bin: sp.exor.o callsp.o appleid.o interface.o video7.o
	$(link-bin)

sp.exor.o: sp.exor.c appleid.h interface.h callsp.h
appleid.o: appleid.c appleid.h
interface.o: interface.c interface.h video7.h
video7.o: video7.c video7.h

.c.o:
	$(CC) -l $(basename $@).lst -c $(CFLAGS) -t $(PLATFORM) $< -o $@

.s.o:
	$(CC) -l $(basename $@).lst -c -t $(PLATFORM) $< -o $@

.bin.o:
	$(CC) -t $(PLATFORM) $< -o $@
