#!/bin/sh

git_root=$(git rev-parse --show-toplevel)
files=$(find ${git_root} -type f -name "*.[ch]")
clang-format -i -style=file:${git_root}/.clang-format ${files}
echo "Clang-Format: Done"
