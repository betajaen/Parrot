PLATFORM = amiga
GAME = maniac
DEBUG = 0
CLI = 0
OBJS = 
PDIR = platform/$(PLATFORM)
GDIR = $(GAME)
CFLAGS = -std=c++17 -I.

include $(PDIR)/makefile.inc
include $(GDIR)/makefile.inc

OBJS += util/string.o

ifneq ($(DEBUG), 0)
	CFLAGS += -g -O0 -DPARROT_RELEASE
else
	CFLAGS += -s -O2 -DPARROT_DEBUG
endif

ifeq ($(GAME), "maniac")
	CFLAGS += -DGAME_NAME="Maniac Mansion" -DGAME=1
endif

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ../game/$(PROGRAM)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean .IGNORE:
	rm $(OBJS)
	rm $(PROGRAM)
