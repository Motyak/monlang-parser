SHELL := /bin/bash
RM := rm -rf
CXXFLAGS := --std=c++23 -Wall -Wextra -Og -ggdb3 -I include -I monlang-LV1
DEPFLAGS = -MMD -MP -MF .deps/$*.d

###########################################################

ENTITIES := \
Program \
Rvalue \
RvalueStatement \
Statement \

OBJS := $(ENTITIES:%=obj/%.o) obj/common.o
DEPS := $(ENTITIES:%=.deps/%.d) .deps/common.d

###########################################################

all: main

main: $(OBJS)

clean:
	$(RM) obj/* .deps/*

mrproper:
	$(RM) obj .deps

.PHONY: all main test check dist clean mrproper

###########################################################

$(OBJS): obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

-include $(DEPS)

.DELETE_ON_ERROR:
.SUFFIXES:

