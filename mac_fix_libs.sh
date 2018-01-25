#!/bin/bash

FILES=$1/*

for f in $FILES
do
  echo "Processing $f file..."

  eval "install_name_tool -id $(basename "$f") $f"

  deps=($( otool -L $f | grep '/usr/local/\|@rpath/\|/Users/\|@executable_path/../Frameworks/lib' | cut -f 1 -d '(' - | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' ))

  for d in "${deps[@]}"
  do
    if [ -f "$d" ]; then
      echo "Dependency: $d..."

      base=$(basename "$d")

      command="install_name_tool -change $d @loader_path/../Frameworks/$base $f"

      #echo "$command"

      eval $command
    fi
  done
done

