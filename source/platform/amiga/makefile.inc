OBJS += platform/amiga/entry.o \
		platform/amiga/string.o \
		platform/amiga/screen.o \
		platform/amiga/timer.o \
		platform/amiga/file.o \
		platform/amiga/log.o


CC			= m68k-amigaos-gcc
DATESTR		= $(shell date +"%-d.%-m.%Y")
CFLAGS		+= -DPARROT_AMIGA -D__AMIGADATE__="\"$(DATESTR)\"" -noixemul -fno-exceptions -m68020