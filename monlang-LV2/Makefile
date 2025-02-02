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

ENTITIES = \
Program \
Statement \
$(STMT_ENTITIES:%=stmt/%) \
Expression \
$(EXPR_ENTITIES:%=expr/%)

STMT_ENTITIES := \
Assignment \
Accumulation \
LetStatement \
VarStatement \
ReturnStatement \
BreakStatement \
ContinueStatement \
DieStatement \
ForeachStatement \
WhileStatement \
ExpressionStatement \

EXPR_ENTITIES := \
Operation \
FunctionCall \
Lambda \
BlockExpression \
SpecialSymbol \
Literal \
Lvalue \

OBJS := $(ENTITIES:%=obj/%.o) obj/precedence.o
DEPS := $(ENTITIES:%=.deps/%.d) .deps/precedence.d

TEST_FILENAMES := $(foreach file,$(wildcard src/test/*.cpp),$(file:src/test/%.cpp=%))
TEST_DEPS := $(TEST_FILENAMES:%=.deps/test/%.d)
TEST_OBJS = $(TEST_FILENAMES:%=obj/test/%.o)
TEST_BINS := $(TEST_FILENAMES:%=bin/test/%.elf)

LIB_ARTIFACT_DIRS := ${foreach lib,${wildcard lib/*/},$(lib:%/=%)/{.deps,obj,dist,bin}} # for cleaning

###########################################################

all: main

main: $(OBJS)

clean:
	$(RM) obj/* .deps/*

mrproper:
	$(RM) obj .deps bin dist lib/libs.a lib/test-libs.a $(LIB_ARTIFACT_DIRS)

.PHONY: all main clean mrproper

###########################################################

$(OBJS): obj/%.o: src/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(DEPFLAGS)

$(TEST_OBJS): obj/test/%.o: src/test/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS_TEST) $(DEPFLAGS_TEST)

$(TEST_BINS): bin/test/%.elf: obj/test/%.o $(OBJS) lib/libs.a lib/test-libs.a
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

dist/monlang-LV2.a: ARFLAGS = rcsvD
dist/monlang-LV2.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

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

## build lib used for testing (catch2) ##
test_libs += lib/catch2/obj/catch_amalgamated.o
test_lib_include_dirs += lib/catch2/include
lib/catch2/obj/catch_amalgamated.o:
	$(MAKE) -C lib/catch2

## build our own lib used for testing (montree) ##
test_libs += lib/montree/dist/montree.a
test_lib_include_dirs += lib/montree/include
$(if $(and $(call not,$(BUILD_LIBS_ONCE)),$(call askmake, lib/montree)), \
	.PHONY: lib/montree/dist/montree.a)
lib/montree/dist/montree.a:
	$(MAKE) -C lib/montree dist/montree.a

###########################################################

# will create all necessary directories after the Makefile is parsed
${call shell_onrun, mkdir -p {.deps,obj}/{stmt,expr,test} dist bin/test}

## debug settings ##
$(call shell_onrun, [ -e bin/test/.gdbinit ] || cp .gdbinit bin/test/.gdbinit)
$(call shell_onrun, grep -qs '^set auto-load safe-path /$$' ~/.gdbinit || echo "set auto-load safe-path /" >> ~/.gdbinit)

## shall not rely on these ##
# .DELETE_ON_ERROR:
.SUFFIXES:
