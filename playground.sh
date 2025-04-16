#!/usr/bin/env bash
set -o errexit

function VERBOSE {
	echo "@"$@
	$@
}

VERBOSE make -C monlang-LV2 bin/test/all.elf

need_rebuild=
for dep in src/test/playground.cpp lib/libs.a lib/test-libs.a montree/dist/montree.a; do
	if [ $dep -nt bin/test/playground.elf ]; then
		need_rebuild=true
		break
	fi
done

if [ "$need_rebuild" == "true" ]; then
VERBOSE ccache g++ \
	-o bin/test/playground.elf \
	src/test/playground.cpp \
	lib/libs.a \
	lib/test-libs.a \
	montree/dist/montree.a \
	--std=c++23 -Wall -Wextra -Og -ggdb3 \
	-I include \
	-I lib/catch2/include \
	-I montree/include
fi

VERBOSE bin/test/playground.elf
