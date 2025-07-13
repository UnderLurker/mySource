#!/usr/bin/bash

if [ "$1" == "-h" -o "$1" == "--help" ]; then
  echo -e "usage foramt:"
  echo -e "build.sh [target] [--fast] compile target"
fi

TARGET_DIR='out/default'
LOG_FILE='build.log'
LAST_ARG="${@: -1}"

if [ $LAST_ARG != "--fast" ]; then
  gn gen $TARGET_DIR --export-compile-commands
fi

ninja -C $TARGET_DIR $1 | tee "$TARGET_DIR/$LOG_FILE"
