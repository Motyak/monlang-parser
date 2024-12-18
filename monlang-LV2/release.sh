#!/bin/bash
shopt -s globstar
set -o errexit

## we expect at least one header file and one obj file ##
(shopt -s failglob; : include/monlang-LV2/**/*.h)
(shopt -s failglob; : obj/*.o)

exec 9> /tmp/monlang_trace.txt
export BASH_XTRACEFD=9
trap '{ echo RELEASE FAILED, trace was saved in /tmp/monlang_trace.txt; } 9>/dev/null' ERR
set -o xtrace

rm -rf dist/monlang-LV2; mkdir -p $_
ar rcsvD dist/monlang-LV2.a "${@:-obj/**/*.o}"
cp -r include/monlang-LV2/* -t dist/monlang-LV2/

echo RELEASE DONE
