#==stuff linked to
DYNAMIC = -lSDL_ttf -lSDL_mixer -lSDL_image -lSDL -lm
#==global Flags. Even on the gp2x with 16 kb Cache, -O3 is much better then -Os
CFLAGS = -fsingle-precision-constant -fPIC
# Testtweaks: -fgcse-lm -fgcse-sm -fsched-spec-load -fmodulo-sched -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fgcse-las -fgcse-after-reload -fvariable-expansion-in-unroller -ftracer -fbranch-target-load-optimize
GENERAL_TWEAKS = -ffast-math
#==PC==
FLAGS = -g -DDESKTOP $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`

SPARROW_FOLDER = ../sparrow3d

SPARROW3D_LIB = libsparrow3d.so
SPARROWNET_LIB = libsparrowNet.so
SPARROWSOUND_LIB = libsparrowSound.so

ifdef TARGET
include $(SPARROW_FOLDER)/target-files/$(TARGET).mk
BUILD = ./build/$(TARGET)/meteoroid3d
SPARROW_LIB = $(SPARROW_FOLDER)/build/$(TARGET)/sparrow3d
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
SPARROW_LIB = $(SPARROW_FOLDER)
endif
LIB += -L$(SPARROW_LIB)
INCLUDE += -I$(SPARROW_FOLDER)
DYNAMIC += -lsparrow3d -lsparrowSound -lsparrowNet

CFLAGS += $(FLAGS) $(PARAMETER)

all: meteoroid3d
	@echo "=== Built for Target "$(TARGET)" ==="

targets:
	@echo "The targets are the same like for sparrow3d. :P"

meteoroid3d: meteoroid3d.c glasses.o stereo.o matrix.o space.o makeBuildDir
	cp -u $(SPARROW_LIB)/$(SPARROW3D_LIB) $(BUILD)
	cp -u $(SPARROW_LIB)/$(SPARROWSOUND_LIB) $(BUILD)
	cp -u $(SPARROW_LIB)/$(SPARROWNET_LIB) $(BUILD)
	$(CC) $(CFLAGS) $(LINK_FLAGS) meteoroid3d.c glasses.o stereo.o matrix.o space.o $(SDL) $(INCLUDE) $(LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/meteoroid3d$(SUFFIX)

glasses.o: glasses.c glasses.h
	$(CC) $(CFLAGS) -c glasses.c $(SDL) $(INCLUDE)

stereo.o: stereo.c stereo.h
	$(CC) $(CFLAGS) -c stereo.c $(SDL) $(INCLUDE)

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c $(SDL) $(INCLUDE)

space.o: space.c space.h
	$(CC) $(CFLAGS) -c space.c $(SDL) $(INCLUDE)

makeBuildDir:
	 @if [ ! -d $(BUILD:/meteoroid3d=/) ]; then mkdir $(BUILD:/meteoroid3d=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

clean:
	rm -f *.o
	rm -f meteoroid3d

oclean:
	rm -f *.o
