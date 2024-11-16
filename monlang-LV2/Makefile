include utils.mk # buildmacros, askmake, not, shell_onrun, shouldrebuild

export CXX := ccache g++

SHELL := /bin/bash
RM := rm -rf
CXXFLAGS := --std=c++23 -Wall -Wextra -Og -ggdb3 -I include -I monlang-LV1
CXXFLAGS_TEST = $(CXXFLAGS) $(addprefix -I ,$(LIB_INCLUDE_DIRS))
DEPFLAGS = -MMD -MP -MF .deps/$*.d
DEPFLAGS_TEST = -MMD -MP -MF .deps/test/$*.d
ARFLAGS := rcsv

BUILD_LIBS_ONCE ?= x # disable by passing `BUILD_LIBS_ONCE=`

###########################################################

ENTITIES := \
Program \
Rvalue \
RvalueStatement \
Statement \

OBJS := $(ENTITIES:%=obj/%.o) obj/common.o obj/context.o obj/context_init.o
DEPS := $(ENTITIES:%=.deps/%.d) .deps/common.d

TEST_FILENAMES := $(foreach file,$(wildcard src/test/*.cpp),$(file:src/test/%.cpp=%))
TEST_DEPS := $(TEST_FILENAMES:%=.deps/test/%.d)
TEST_BINS := $(TEST_FILENAMES:%=bin/test/%.elf)

LIB_OBJ_DIRS := $(foreach lib,$(wildcard lib/*/),$(lib:%/=%)/obj) # for cleaning
LIB_INCLUDE_DIRS := $(foreach lib,$(wildcard lib/*/),$(lib:%/=%)/include)

###########################################################

all: main

main: $(OBJS)

clean:
	$(RM) obj/* .deps/*

mrproper:
	$(RM) obj .deps bin lib/test-libs.a $(LIB_OBJ_DIRS)

.PHONY: all main clean mrproper

###########################################################

$(OBJS): obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

$(TEST_BINS): bin/test/%.elf: src/test/%.cpp $(OBJS) monlang-LV1/monlang-LV1.a lib/test-libs.a
	$(CXX) -o $@ $< $(OBJS) monlang-LV1/monlang-LV1.a lib/test-libs.a $(CXXFLAGS_TEST) $(DEPFLAGS_TEST) $(LDFLAGS) $(LDLIBS)

-include $(DEPS) $(TEST_DEPS)

############################################################
# libs
############################################################

## aggregate all test lib objects into one static test lib ##
.SECONDEXPANSION:
lib/test-libs.a: $$(test_lib_objects)
# when BUILD_LIBS_ONCE is unset => we always enter this recipe
	$(if $(call shouldrebuild, $@, $^), \
		$(AR) $(ARFLAGS) $@ $^)

## compiles lib used for testing (catch2) ##
test_lib_objects += lib/catch2/obj/catch_amalgamated.o
lib/catch2/obj/catch_amalgamated.o:
	$(MAKE) -C lib/catch2

## compiles our own lib used for testing (montree) ##
test_lib_objects += lib/montree/obj/montree.o
$(if $(and $(call not,$(BUILD_LIBS_ONCE)),$(call askmake, lib/montree)), \
	.PHONY: lib/montree/obj/montree.o)
lib/montree/obj/montree.o:
	$(MAKE) -C lib/montree

###########################################################

# will create all necessary directories after the Makefile is parsed
$(call shell_onrun, mkdir -p obj obj/test .deps/test bin/test $(LIB_OBJ_DIRS))

## debug settings ##
$(call shell_onrun, [ ! -e bin/test/.gdbinit ] && cp .gdbinit bin/test/.gdbinit)
$(call shell_onrun, grep -qs '^set auto-load safe-path /$$' ~/.gdbinit || echo "set auto-load safe-path /" >> ~/.gdbinit)

.DELETE_ON_ERROR:
.SUFFIXES:
