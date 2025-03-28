include utils.mk # askmake, not, shell_onrun, shouldrebuild

export CXX := ccache g++

SHELL := /bin/bash
RM := rm -rf
CXXFLAGS = --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
CXXFLAGS_TEST = $(CXXFLAGS) $(addprefix -I ,$(test_lib_include_dirs))
DEPFLAGS = -MMD -MP -MF .deps/$*.d
DEPFLAGS_TEST = -MMD -MP -MF .deps/test/$*.d
ARFLAGS = D -M < <(tools/aggregate-libs.mri.sh $@ $^); :

###########################################################

OBJS := obj/parse.o \
obj/ReconstructLV1Tokens.o obj/LV1EntityMap.o \
obj/ReconstructLV2Tokens.o obj/LV2EntityMap.o
DEPS := $(OBJS:obj/%.o=.deps/%.d)

TEST_FILENAMES := $(foreach file,$(wildcard src/test/*.cpp),$(file:src/test/%.cpp=%))
TEST_DEPS := $(TEST_FILENAMES:%=.deps/test/%.d)
TEST_OBJS = $(TEST_FILENAMES:%=obj/test/%.o)
TEST_BINS := $(TEST_FILENAMES:%=bin/test/%.elf)

LIB_ARTIFACT_DIRS := ${foreach lib,${wildcard lib/*/},$(lib:%/=%)/{.deps,obj,dist,bin}} # for cleaning

###########################################################

all: dist/monlang-parser.a

main: $(OBJS) lib/libs.a

clean:
	$(RM) obj/* .deps/*

mrproper:
	$(RM) obj .deps bin/*.elf bin/out bin/test dist lib/libs.a lib/test-libs.a $(LIB_ARTIFACT_DIRS)

.PHONY: all main dist clean mrproper

###########################################################

$(OBJS) obj/main.o: obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

$(TEST_OBJS): obj/test/%.o: src/test/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS_TEST) $(DEPFLAGS_TEST)

$(TEST_BINS): bin/test/%.elf: obj/test/%.o $(OBJS) lib/libs.a lib/test-libs.a
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

dist/monlang-parser.a: ARFLAGS = rcsvD
dist/monlang-parser.a: $(OBJS) lib/libs.a # montree/dist/montree.a probably?
#                                             -> the module using monlang-parser shall also use montree
	$(AR) $(ARFLAGS) $@ $^

bin/main.elf: ADDITIONAL_CXXFLAGS = -Wno-unused-label -I montree/include -MMD -MP -MF .deps/main.d
bin/main.elf: src/main.cpp $(OBJS) lib/libs.a montree/dist/montree.a
	$(CXX) -o $@ src/main.cpp $(OBJS) lib/libs.a montree/dist/montree.a $(CXXFLAGS) $(ADDITIONAL_CXXFLAGS)

-include $(DEPS) $(TEST_DEPS) .deps/main.d

############################################################
# libs
############################################################

## aggregate all main code dependencies (.o, .a) into one static lib ##
.SECONDEXPANSION:
lib/libs.a: $$(libs)
	$(if $(call shouldrebuild, $@, $^), \
		$(AR) $(ARFLAGS) $@ $^)

## aggregate all test code dependencies (.o, .a) into one static lib ##
.SECONDEXPANSION:
lib/test-libs.a: $$(test_libs)
	$(if $(call shouldrebuild, $@, $^), \
		$(AR) $(ARFLAGS) $@ $^)

# lib monlang-LV1 (manually built and updated when desired..
# .., additional release from monlang/)
libs += lib/monlang-LV1/dist/monlang-LV1.a

## build lib monlang-LV2 ##
libs += lib/monlang-LV2/dist/monlang-LV2.a
$(if $(call askmake, lib/monlang-LV2), \
	.PHONY: lib/monlang-LV2/dist/monlang-LV2.a)
lib/monlang-LV2/dist/monlang-LV2.a:
	$(MAKE) -C lib/monlang-LV2 dist/monlang-LV2.a

## build lib used for testing (catch2) ##
test_libs += lib/catch2/obj/catch_amalgamated.o
test_lib_include_dirs += lib/catch2/include
lib/catch2/obj/catch_amalgamated.o:
	$(MAKE) -C lib/catch2

## build lib montree used in main.elf only ##
$(if $(call askmake, montree), \
	.PHONY: montree/dist/montree.a)
montree/dist/montree.a:
	$(MAKE) -C montree dist/montree.a

###########################################################

# will create all necessary directories after the Makefile is parsed
${call shell_onrun, mkdir -p {.deps,obj,bin}/test dist}

## debug settings ##
$(call shell_onrun, [ -e bin/test/.gdbinit ] || cp .gdbinit bin/test/.gdbinit)
$(call shell_onrun, grep -qs '^set auto-load safe-path /$$' ~/.gdbinit || echo "set auto-load safe-path /" >> ~/.gdbinit)

## shall not rely on these ##
# .DELETE_ON_ERROR:
.SUFFIXES:
