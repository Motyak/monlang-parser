#!/bin/bash

trap 'rm filelist.txt' EXIT

# add monlang-LV1/ files
find monlang-LV1/ -name '*.cpp' -o -name '*.h' -o -name 'Makefile' -o -name '*.mk' -o -name '*.sh' | grep -v '/dist/' | grep -v 'catch' | grep -v 'montree' | grep -v 'handleterm' | grep -v '/utils' | awk '{print "./"$0}' > filelist.txt

# add monlang-LV2/ and montree/ files
find . -name '*.cpp' -o -name '*.h' -o -name 'Makefile' -o -name '*.mk' -o -name '*.sh' | grep -v 'catch' | grep -v 'montree/utils.mk' | grep -v 'monlang-LV2/utils.mk' | grep -v 'monlang-LV2/tools/aggregate-libs.mri.sh' >> filelist.txt

#cat filelist.txt | grep -vi 'test' > .filelist.txt
#mv .filelist.txt filelist.txt

var="$(cloc --list-file=filelist.txt --by-file)"
sum_row="$(tail -n2 <<< "$var" | head -n2)"
table="$(tail -n+2 <<< "$var")"
tac <<< "$table"
echo "$sum_row"
