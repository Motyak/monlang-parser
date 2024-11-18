SHELL := /bin/bash
RM := rm -rf
CXXFLAGS := --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
DEPFLAGS = -MMD -MP -MF .deps/$(notdir $*.d)

ifdef CLANG
CXX := clang++
# ugly workaround to support clang
CXXFLAGS += -D__cpp_concepts=202002L
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -m32
endif

###########################################################

SRC_NAMES := montree
OBJS := $(SRC_NAMES:%=obj/%.o)
DEPS := $(SRC_NAMES:%=.deps/%.d)

###########################################################

all: build

build: $(OBJS)

clean:
	$(RM) $(OBJS) $(DEPS)

mrproper:
	$(RM) obj .deps


.PHONY: all build clean mrproper

###########################################################

$(OBJS): obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

-include $(DEPS)

###########################################################

# will create all necessary directories after the Makefile is parsed
$(shell mkdir -p obj .deps)
