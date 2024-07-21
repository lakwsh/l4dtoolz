PROJECT = l4dtoolz
OBJECTS = l4dtoolz.cpp sigtool/mem.cpp

CPP = gcc

HL2SDK = ../hl2sdk-l4d2
HL2PUB = $(HL2SDK)/public

LIB_EXT = so
HL2LIB = $(HL2SDK)/lib/linux
LIB_PREFIX = lib
LIB_SUFFIX = _srv.$(LIB_EXT)

LINK += $(HL2LIB)/tier1_i486.a $(LIB_PREFIX)vstdlib$(LIB_SUFFIX) $(LIB_PREFIX)tier0$(LIB_SUFFIX)

INCLUDE += -I. -Isigtool -I$(HL2PUB)/engine -I$(HL2PUB)/game/server \
	-I$(HL2PUB) -I$(HL2PUB)/vstdlib -I$(HL2PUB)/tier0 -I$(HL2PUB)/tier1

BINARY = $(PROJECT).$(LIB_EXT)
BIN_DIR = Release
LINK += -shared

CFLAGS += -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -D_snprintf=snprintf \
	-Dstrnicmp=strncasecmp -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp \
	-D_LINUX -std=c++11 -Wall -Werror -fPIC -fno-exceptions -fno-rtti -msse -m32 -Ofast -pipe \
	-fvisibility=hidden -fvisibility-inlines-hidden -static-libgcc -fno-strict-aliasing

OBJ_BIN := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)/sigtool
	ln -sf $(HL2LIB)/$(LIB_PREFIX)vstdlib$(LIB_SUFFIX)
	ln -sf $(HL2LIB)/$(LIB_PREFIX)tier0$(LIB_SUFFIX)
	$(MAKE) -f Makefile l4dtoolz

l4dtoolz: $(OBJ_BIN)
	$(CPP) $(INCLUDE) -m32 $(OBJ_BIN) $(LINK) -ldl -lm -s -o $(BIN_DIR)/$(BINARY)

default: all

clean:
	rm -rf $(BIN_DIR)
	rm -f *$(LIB_SUFFIX)
