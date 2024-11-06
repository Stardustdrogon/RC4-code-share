#!/bin/bash

filename="$1"
content="$2"
vpd_mount="/vpd"
temp_dir="/tmp/sda4_mount"   
temp_data="/tmp/vpd_data"
squashfs_device="/dev/sda4"
squashfs_image="/tmp/vpd.sqsh"

#参数检查
check_usage() {
  if [ "$#" -ne 2 ];then
    echo "Usage:$0 <filename> <content>"
    exit 1
  fi
}

#文件更新、生成squashfs镜像
update_squashfs() {
  sudo mount -t squashfs "$squashfs_device" "$temp_dir" -o ro #将squashfs镜像挂载到临时目录

  mkdir -p "$temp_data" 

  sudo cp -a "$temp_dir"/* "$temp_data" #将sda4中的文件复制到临时目录

  sudo umount "$temp_dir"

  echo "$content" > "$temp_data/$filename"

  sudo mksquashfs "$temp_data" "$squashfs_image" -comp xz -noappend #将临时目录打包成squashfs镜像

}
#写入squashfs镜像到sda4
write_squashfs_to_device() {
  sudo dd if="$squashfs_image" of="$squashfs_device" bs=4M conv=fdatasync
}
#挂载sda4到vpd
mount_device_to_vpd() {
  sudo mount -t squashfs "$squashfs_device" "$vpd_mount" -o ro
}

cleanup() {
  sudo rm -rf "$temp_dir" 
  sudo rm -rf "$temp_data"
  sudo rm -f "$squashfs_image"
}

main() {
  
  check_usage "$@"
  
  mkdir -p "$temp_dir"

  if mountpoint -q "$vpd_mount";then
    sudo umount "$vpd_mount"
  fi
  
  update_squashfs 
  write_squashfs_to_device
  mount_device_to_vpd
  cleanup
}

main "$@"

