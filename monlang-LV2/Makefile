include utils.mk # buildmacros, askmake, not, shell_onrun, shouldrebuild

export CXX := ccache g++

SHELL := /bin/bash
RM := rm -rf
CXXFLAGS := --std=c++23 -Wall -Wextra -Og -ggdb3 -I include -I monlang-LV1
CXXFLAGS_TEST = $(CXXFLAGS) $(addprefix -I ,$(LIB_INCLUDE_DIRS))
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
Accumulation \
Assignment \
BreakStatement \
ContinueStatement \
DieStatement \
ExpressionStatement \
ForeachStatement \
LetStatement \
ReturnStatement \
VarStatement \

EXPR_ENTITIES := \
Operation \
FunctionCall \
Lambda \
BlockExpression \
Literal \
Lvalue \

OBJS := $(ENTITIES:%=obj/%.o) obj/context.o obj/context_init.o obj/precedence.o
DEPS := $(ENTITIES:%=.deps/%.d) .deps/context.d .deps/context_init.d .deps/precedence.d

TEST_FILENAMES := $(foreach file,$(wildcard src/test/*.cpp),$(file:src/test/%.cpp=%))
TEST_DEPS := $(TEST_FILENAMES:%=.deps/test/%.d)
TEST_OBJS = $(TEST_FILENAMES:%=obj/test/%.o)
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

$(TEST_OBJS): obj/test/%.o: src/test/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS_TEST) $(DEPFLAGS_TEST)

# test-libs.a must appear before monlang-LV1.a because montree depends on LV1
$(TEST_BINS): bin/test/%.elf: obj/test/%.o $(OBJS) lib/test-libs.a lib/monlang-LV1.a
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

-include $(DEPS) $(TEST_DEPS)

############################################################
# libs
############################################################

## aggregate all test lib (.o, .a) into one static lib ##
.SECONDEXPANSION:
lib/test-libs.a: $$(test_libs)
# when BUILD_LIBS_ONCE is unset => we always enter this recipe
	$(if $(call shouldrebuild, $@, $^), \
		$(AR) $(ARFLAGS) $@ $^)

## compiles lib used for testing (catch2) ##
test_libs += lib/catch2/obj/catch_amalgamated.o
lib/catch2/obj/catch_amalgamated.o:
	$(MAKE) -C lib/catch2

## compiles our own lib used for testing (montree) ##
test_libs += lib/montree/dist/montree.a
$(if $(and $(call not,$(BUILD_LIBS_ONCE)),$(call askmake, lib/montree)), \
	.PHONY: lib/montree/dist/montree.a)
lib/montree/dist/montree.a:
	$(MAKE) -C lib/montree

###########################################################

# will create all necessary directories after the Makefile is parsed
${call shell_onrun, mkdir -p {obj,.deps}/{stmt,expr,test} bin/test ${LIB_OBJ_DIRS}}

## debug settings ##
$(call shell_onrun, [ ! -e bin/test/.gdbinit ] && cp .gdbinit bin/test/.gdbinit)
$(call shell_onrun, grep -qs '^set auto-load safe-path /$$' ~/.gdbinit || echo "set auto-load safe-path /" >> ~/.gdbinit)

# .DELETE_ON_ERROR:
.SUFFIXES:
