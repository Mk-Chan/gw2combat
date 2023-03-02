CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -Wno-deprecated -pipe -Isrc/ -Iinclude/ $(EXTRACXXFLAGS)
LDFLAGS = -pthread $(CXXFLAGS) $(EXTRALDFLAGS)

SRCS = src/main.cpp src/system/encounter.cpp src/system/temporal.cpp src/system/actor.cpp src/system/attributes.cpp src/system/rotation.cpp src/system/effects.cpp src/system/dispatch_strikes_and_effects.cpp src/system/apply_strikes_and_effects.cpp src/system/audit.cpp src/combat_loop.cpp src/server.cpp
OBJS = $(SRCS:.cpp=.o)

EXE = gw2combat

ifeq ($(BUILD),debug)
	CXXFLAGS += -g -fno-omit-frame-pointer -DDEBUG
else
	CXXFLAGS += -O3 -DNDEBUG
endif

UNAME = $(shell uname)

ifeq ($(UNAME),Linux)
	CXXFLAGS += -static
else
	ifeq ($(UNAME),Darwin)
	else
		CXXFLAGS += -static -lwsock32 -lws2_32 -m64 -Wa,-mbig-obj
	endif
endif

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	-rm -f $(OBJS) $(EXE)
