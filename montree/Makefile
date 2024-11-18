CXXFLAGS := --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
DEPFLAGS = -MMD -MP -MF .deps/$(notdir $*.d)
RM := rm -rf

ifdef CLANG
CXX := clang++
# ugly workaround to support clang
CXXFLAGS += -D__cpp_concepts=202002L
endif

ifdef X86
CXXFLAGS += -m32
endif

###########################################################

all: build

build: obj/montree.o

clean:
	$(RM) obj/montree.o .deps/montree.d

mrproper:
	$(RM) obj .deps


.PHONY: all build clean mrproper

###########################################################

obj/montree.o: src/montree.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

-include .deps/montree.d

###########################################################

# will create all necessary directories after the Makefile is parsed
$(shell mkdir -p obj .deps)
