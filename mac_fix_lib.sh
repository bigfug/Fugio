#!/bin/bash

MACDEPLOYQT=$1
BUNDLE_DIR=$2
BUNDLE_EXE=$3

$1 "$BUNDLE_DIR" -no-plugins -verbose=1

for f in "$BUNDLE_DIR/Contents/Frameworks"/Qt*.framework
do
  echo "Deleting ${f}..."

  rm -rf ${f}
done

for f in $BUNDLE_DIR/Contents/MacOS/$BUNDLE_EXE
do
  echo "Processing ${f} file..."

  eval "install_name_tool -id $(basename "${f}") ${f}"

  deps=($( otool -L "${f}" | grep '/usr/local/\|/Users/\|@executable_path/../Frameworks/lib' | cut -f 1 -d '(' - | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' ))

  for d in "${deps[@]}"
  do
    if [ -n "$d" ]; then
      echo "Dependency: $d..."

      base=$(basename "$d")

      command="install_name_tool -change $d @loader_path/../Frameworks/$base ${f}"

      #echo "$command"

      eval $command
    fi
  done
done

