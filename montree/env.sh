[ "${BASH_SOURCE[0]}" == "$0" ] && { >&2 echo "this script is meant to be sourced, not executed"; exit 1; }

# detect broken symbolic links
find . -xtype l -exec echo 'WARN: Broken symlink: {}' \;

alias make='make -j16'
