
### 问题描述：
```
    system 有一個 partition, 目前規劃, 用來存放永久性資料,所以 partition 
會使用 readonly file system,例如 romfs, cramfs, squashfs,系統開機的
script 會 mount 這個 partition 到 /vpd 的 directory但是如果這個 
partition 還沒有被寫入第一次資料時,系統 mount 的動作會失敗
    請設計兩個 shell script, 一個用來 update /vpd 裏頭的檔案,
一個用來讀取 /vpd 裏頭檔案
update_vpd.sh param1 param2
   param1 : 字串會被當作是 file name, update 到 /vpd 中
   param2 : param1 指定file name的內容
   
   例如 update_vpd.sh "mac1" "aa:bb:cc:dd:ee:ff",
   就會在 /vpd 中 update "mac1" 這個 file name 的 file,
   file 的內容是 "aa:bb:cc:dd:ee:ff"
   這邊叫做 update 的意思, 是 /vpd 下有可能已經有相同 file name,
   也有可能沒有相同 file name, 如果已經有相同 file name,
   就是更新這個 file name 的內容; 如果沒有相同 file name,
   則是 create 這個 file name 並指定其內容
read_vpd.sh param1
   param1 : 字串會被當作是 file name, 讀取 /vpd 中的 file name的內容
            如果 /vpd 中沒有這個 file name, 會傳回空字串
                             
   例如 /vpd 中已經有 "mac1" file, 則 read_vpd.sh "mac1"
   會傳回 "mac1" file 的內容
   如果 /vpd 中沒有 "mac1" file, 則 read_vpd.sh "mac1" 會傳回空字串
```


### 环境
- linux系统（Ubuntu22.04），使用Bash shell
### 1.创建一个新分区——sda4
- 使用gparted工具，划分新分区，文件系统为ext4
### 2.将新分区格式化为squashfs
#### (1).创建临时目录 /mnt/sda4 挂载新分区sda4
- `sudo mount /dev/sda4 /mnt/sda4`
#### (2).创建临时目录 /mnt/sda4_data ，将sda4内容复制到/mnt/sda4_data
- `sudo cp -a /mnt/sda4 /mnt/sda4_data`
#### (3).将/mnt/sda4_data打包成squashfs
- `sudo mksquashfs /mnt/sda4_data /mnt/sda4/sda4_data.sqsh -comp xz -noappend`
#### (4).将squashfs镜像写入/dev/sda4
- `sudo dd if=/mnt/sda4/sda4_data.sqsh of=/dev/sda4 bs=4M conv=fdatasync`
#### (5).清理临时目录
- `sudo umount /mnt/sda4`
- `sudo rm -rf /mnt/sda4_data`
- `sudo rm -rf /mnt/sda4`
### 3.创建目录/vpd 挂载新分区sda4——开机自动挂载
- `sudo mount -t squashfs /dev/sda4 /vpd -o ro`
- 在/etc/fstab末尾添加：`/dev/sda4 /vpd squashfs defaults 0 0`
- **tips:此时sda4已是squashfs**
![image.png](https://img.picui.cn/free/2024/11/05/6729be37ad002.png)
![image-3.png](https://img.picui.cn/free/2024/11/05/6729be88bfbfd.png)
### 4.update_vpd.sh
```c
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

  sudo cp -a "$temp_dir" "$temp_data" #将sda4中的文件复制到临时目录

  sudo umount "$temp_dir"
  #文件更新
  echo "$content" > "$temp_data/$filename"
  #将临时目录打包成squashfs镜像
  sudo mksquashfs "$temp_data" "$squashfs_image" -comp xz -noappend > /dev/null 2>&1

}
#写入squashfs镜像到sda4
write_squashfs_to_device() {
  sudo dd if="$squashfs_image" of="$squashfs_device" bs=4M conv=fdatasync > /dev/null 2>&1
}
#挂载sda4到/vpd
mount_device_to_vpd() {
  sudo mount -t squashfs "$squashfs_device" "$vpd_mount" -o ro
}
#清理临时目录
cleanup() {
  sudo rm -rf "$temp_dir" 
  sudo rm -rf "$temp_data"
  sudo rm -f "$squashfs_image"
}

main() {
  check_usage "$@"
  
  mkdir -p "$temp_dir"
  #卸载/vpd挂载点
  if mountpoint -q "$vpd_mount";then
    sudo umount "$vpd_mount"
  fi
  
  update_squashfs 
  write_squashfs_to_device
  mount_device_to_vpd
  cleanup
}

main "$@"
```
### 5.read_vpd.sh
```c

#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage:$0 <filename>"
  exit 1
fi

filename="$1"
vpd_mount="/vpd"
if [ -e "$vpd_mount/$1" ]; then
  cat "$vpd_mount/$1"
else
  echo ""
fi
```
### 实例：
#### 更新文件
![image-1.png](https://img.picui.cn/free/2024/11/05/6729be608d611.png)
#### 新增文件
![image-2.png](https://img.picui.cn/free/2024/11/05/6729be7959d70.png)










