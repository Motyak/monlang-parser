#!/bin/bash
shopt -s globstar
set -o errexit

## we expect at least one header file and one obj file ##
(shopt -s failglob; : include/montree/**/*.h)
(shopt -s failglob; : obj/*.o)

exec 9> /tmp/monlang_trace.txt
export BASH_XTRACEFD=9
trap '{ echo RELEASE FAILED, trace was saved in /tmp/monlang_trace.txt; } 9>/dev/null' ERR
set -o xtrace

rm -rf dist/montree; mkdir -p $_
ar rcsvD dist/montree.a "${@:-obj/**/*.o}"
cp -r include/montree/* -t dist/montree/

echo RELEASE DONE
