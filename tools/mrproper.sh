#!/bin/bash

TARGET_DIR=${1:-.}

makefiles="$(find -L "$TARGET_DIR" -name 'Makefile' | grep -v /lib/ | sort)"
for file in $makefiles; do
    commands="${commands}${commands:+$'\n'}(cd \"${file%/*}\" && make mrproper)"
done

[ "$commands" != "" ] || { >&2 echo "no makefiles were found in \`$TARGET_DIR\`"; exit; }

>&2 echo "about to run the following commands:"
>&2 echo "$commands"
>&2 echo -n "proceed?(Y/n) >"
read confirm
[[ "$confirm" =~ n|N ]] && >&2 echo "aborted" && exit
bash -x -c "$commands"

>&2 echo "done"
