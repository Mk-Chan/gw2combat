CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -Wno-deprecated -pipe -Isrc/ -Iinclude/ $(EXTRACXXFLAGS)
LDFLAGS = -pthread $(CXXFLAGS) $(EXTRALDFLAGS)

SRCS = src/main.cpp src/system/encounter.cpp src/system/temporal.cpp src/system/actor.cpp src/system/effects.cpp src/system/dispatch_strikes_and_effects.cpp src/system/apply_strikes_and_effects.cpp src/system/audit.cpp src/combat_loop.cpp src/server.cpp
OBJS = $(SRCS:.cpp=.o)

EXE = gw2combat

ifeq ($(BUILD),debug)
	CXXFLAGS += -g -fno-omit-frame-pointer -DDEBUG
else
	CXXFLAGS += -O3 -DNDEBUG
endif

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	-rm -f $(OBJS) $(EXE)
