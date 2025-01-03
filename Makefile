include utils.mk # askmake, not, shell_onrun, shouldrebuild

export CXX := ccache g++

SHELL := /bin/bash
RM := rm -rf
CXXFLAGS = --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
CXXFLAGS_TEST = $(CXXFLAGS) $(addprefix -I ,$(test_lib_include_dirs))
DEPFLAGS = -MMD -MP -MF .deps/$*.d
DEPFLAGS_TEST = -MMD -MP -MF .deps/test/$*.d
ARFLAGS = D -M < <(tools/aggregate-libs.mri.sh $@ $^); :

BUILD_LIBS_ONCE ?= x # disable by passing `BUILD_LIBS_ONCE=`

###########################################################

OBJS := obj/parse.o
BINS := bin/main.elf

TEST_FILENAMES := $(foreach file,$(wildcard src/test/*.cpp),$(file:src/test/%.cpp=%))
TEST_DEPS := $(TEST_FILENAMES:%=.deps/test/%.d)
TEST_OBJS = $(TEST_FILENAMES:%=obj/test/%.o)
TEST_BINS := $(TEST_FILENAMES:%=bin/test/%.elf)

###########################################################

all: main

main: bin/main.elf

dist: $(OBJS) lib/libs.a
	./release.sh $^

clean:
	$(RM) obj/* .deps/*

mrproper:
	$(RM) obj .deps bin dist lib/libs.a lib/test-libs.a

.PHONY: all main dist clean mrproper

###########################################################

$(OBJS) obj/main.o: obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

$(BINS): bin/%.elf: obj/%.o $(OBJS) lib/libs.a
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(TEST_OBJS): obj/test/%.o: src/test/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS_TEST) $(DEPFLAGS_TEST)

$(TEST_BINS): bin/test/%.elf: obj/test/%.o $(OBJS) lib/libs.a lib/test-libs.a
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

-include $(DEPS) $(TEST_DEPS)

############################################################
# libs
############################################################

## aggregate all main code dependencies (.o, .a) into one static lib ##
.SECONDEXPANSION:
lib/libs.a: $$(libs)
# when BUILD_LIBS_ONCE is unset => we always enter this recipe
	$(if $(call shouldrebuild, $@, $^), \
		$(AR) $(ARFLAGS) $@ $^)

## aggregate all test code dependencies (.o, .a) into one static lib ##
.SECONDEXPANSION:
lib/test-libs.a: $$(test_libs)
# when BUILD_LIBS_ONCE is unset => we always enter this recipe
	$(if $(call shouldrebuild, $@, $^), \
		$(AR) $(ARFLAGS) $@ $^)

# lib monlang-LV1 (manually built and updated when desired..
# .., additional release from monlang/)
libs += lib/monlang-LV1/dist/monlang-LV1.a

## build lib monlang-LV2 ##
libs += lib/monlang-LV2/dist/monlang-LV2.a
$(if $(and $(call not,$(BUILD_LIBS_ONCE)),$(call askmake, lib/monlang-LV2)), \
	.PHONY: lib/monlang-LV2/dist/monlang-LV2.a)
lib/monlang-LV2/dist/monlang-LV2.a:
	$(MAKE) -C lib/monlang-LV2 dist/monlang-LV2.a

## build lib used for testing (catch2) ##
test_libs += lib/catch2/obj/catch_amalgamated.o
test_lib_include_dirs += lib/catch2/include
lib/catch2/obj/catch_amalgamated.o:
	$(MAKE) -C lib/catch2

###########################################################

# will create all necessary directories after the Makefile is parsed
${call shell_onrun, mkdir -p {.deps,obj,bin}/test dist}

## debug settings ##
$(call shell_onrun, [ -e bin/test/.gdbinit ] || cp .gdbinit bin/test/.gdbinit)
$(call shell_onrun, grep -qs '^set auto-load safe-path /$$' ~/.gdbinit || echo "set auto-load safe-path /" >> ~/.gdbinit)

## shall not rely on these ##
# .DELETE_ON_ERROR:
.SUFFIXES:
