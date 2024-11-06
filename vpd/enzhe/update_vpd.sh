#!/bin/bash

SQUASHFS_PARTITION="/dev/sda4"
MOUNT_POINT="/mnt/sda4"
TMP_DIR="/tmp/sda4"
IMAGE_DIR="/tmp/image"
NEW_IMAGE_PATH="/tmp/image/update_sda4.sqsh"

FILENAME="$1"
CONTENT="$2"

# 初始化创建目录
function Init()
{
    sudo mkdir -p "$MOUNT_POINT"
    sudo mkdir -p "$TMP_DIR"
    sudo mkdir -p "$IMAGE_DIR"
}

# 拷贝vpd资料
function Copy()
{
    sudo umount /vpd

    sudo mount -t squashfs "$SQUASHFS_PARTITION" "$MOUNT_POINT" -o ro

    sudo cp -a "$MOUNT_POINT"/* "$TMP_DIR"

    sudo umount "$MOUNT_POINT"
}

# 更新文件
function Update()
{
    echo "$CONTENT" | sudo tee "$TMP_DIR/$FILENAME" > /dev/null

    sudo mksquashfs "$TMP_DIR" "$NEW_IMAGE_PATH" -comp xz #-noappend

    sudo dd if="$NEW_IMAGE_PATH" of="$SQUASHFS_PARTITION" bs=4M status=progress

    sudo mount -t squashfs "$SQUASHFS_PARTITION" /vpd -o ro
}

# 删除临时文件
function Delete()
{
    sudo rm "$NEW_IMAGE_PATH"
    sudo rm -rf "$MOUNT_POINT"
    sudo rm -rf "$TMP_DIR"
}

main()
{
    # 帮助选项
    if [ "$1" == "-h" ]; then
        echo "usage: $0 <filename> <text> or $0 -h"
        echo "example:$0 example.txt 'this is message'"
        exit 0
    fi

    # 参数检查
    if [ "$#" -ne 2 ]; then
        echo "usage: $0 <filename> <text>"
        exit 1
    fi
    
    Init
    Copy
    Update
    Delete
}

main "$@"
