PLATFORM=apple2enh
CC=cl65
CFLAGS=-O

define single-to-double
  unsingle $< && mv $<.ad $@ && mv .AppleDouble/$<.ad .AppleDouble/$@
endef

define link-bin
  $(CC) -m $@.map -vm -t $(PLATFORM) $(LDFLAGS) $(LIBS) $^ -o $@
endef

all: sp.exor

sp.exor: sp.exor.bin
	$(single-to-double)
sp.exor.bin: sp.exor.o callsp.o
	$(link-bin)

.c.o:
	$(CC) -l $(basename $@).lst -c $(CFLAGS) -t $(PLATFORM) $< -o $@

.s.o:
	$(CC) -l $(basename $@).lst -c -t $(PLATFORM) $< -o $@

.bin.o:
	$(CC) -t $(PLATFORM) $< -o $@
