include utils.mk # shell_onrun

CXXFLAGS := --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
DEPFLAGS = -MMD -MP -MF .deps/$*.d
RM := rm -rf
ARFLAGS := rcsvD

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

SRC_FILENAMES := \
montree-LV1 \
montree-LV2 \
LV1AstBuilder \
LV1AstGenerator \
PrintLV1 \
PrintLV2 \

DEPS := $(SRC_FILENAMES:%=.deps/%.d)
OBJS := $(SRC_FILENAMES:%=obj/%.o)

###########################################################

all: main

main: $(OBJS)

clean:
	$(RM) obj/* .deps/*

mrproper:
	$(RM) obj .deps


.PHONY: all main clean mrproper

###########################################################

$(OBJS): obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

dist/montree.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

-include $(DEPS)

###########################################################

# will create all necessary directories after the Makefile is parsed
$(call shell_onrun, mkdir -p obj .deps dist)
