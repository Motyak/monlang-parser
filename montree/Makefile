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

SRC_FILENAMES := \
montree \
LV1AstGenerator \
PrintLV1 \
PrintLV2 \

DEPS := $(SRC_FILENAMES:%=.deps/%.d)
OBJS := $(SRC_FILENAMES:%=obj/%.o)

###########################################################

all: build

build: obj/montree.o

clean:
	$(RM) obj/* .deps/*

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
