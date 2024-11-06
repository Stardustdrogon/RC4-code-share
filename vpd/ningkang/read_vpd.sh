#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage:$0 <filename> "
  exit 1
fi


vpd_mount="/vpd"
if [ -e "$vpd_mount/$1" ]; then
  cat "$vpd_mount/$1"
else
  echo ""
fi

