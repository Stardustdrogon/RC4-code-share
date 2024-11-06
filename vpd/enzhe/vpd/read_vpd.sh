#!/bin/bash

# 参数检查
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 filename"
    exit 1
fi

FILENAME="$1"
MOUNT_POINT="/vpd"

# 打印文件内容
if [ -e "$MOUNT_POINT/$FILENAME" ]; then
    cat "$MOUNT_POINT/$FILENAME"
else
    echo "File not found."
fi
