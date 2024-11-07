#!/bin/sh

# Check if the first argument (keyword) is provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 <keyword> <value>"
    exit 1  # Exit if the argument count is not equal to 2
fi

# Check if the script is run with sudo
if [ "$EUID" -ne 0 ]; then
  echo "Please run this script with superuser."
  exit 1
fi


# Assign arguments to variables
keyword=$1
value=$2

# Function to update vpd partition and handle mount/unmount operations
update_vpd_partation() {
    echo "Updating VPD image..."
    # Create vpd.img from /var/vpd
    /usr/sbin/mksquashfs /var/vpd /var/vpd.img -no-duplicates -no-recovery > /dev/null

    # Unmount /vpd
    umount /vpd

    # Set the device to read-only mode
    echo 0 > /sys/block/mmcblk0boot0/force_ro

    # Write vpd.img to /dev/mmcblk0boot0 at the specified offset
    dd if=/var/vpd.img of=/dev/mmcblk0boot0 seek=32768 > /dev/null 2>&1

    # Set the device back to read-write mode
    echo 1 > /sys/block/mmcblk0boot0/force_ro

    # Mount the new vpd image
    mount -t squashfs -o offset=$((512*32768)) /dev/mmcblk0boot0 /vpd > /dev/null
    # Clean up: remove /var/vpd and vpd.img
    rm -rf /var/vpd
    rm -f /var/vpd.img
}

update_vpd_value() {
    # Check if the file exists in /var/vpd
    if [ ! -f "/var/vpd/$keyword" ]; then
        echo "File '$keyword' does not exist in /vpd. Creating new file."
        touch "/var/vpd/$keyword"  # Create the file if it does not exist
    fi

    # Write the value to the specified file
    echo "$value" > "/var/vpd/$keyword"
}

verify_vpd_value() {
    # Check if the content in /vpd for $keyword matches $value
    if [ "$(cat /vpd/$keyword)" == "$value" ]; then
        echo "Update successful: /vpd/$keyword value is $value."
    else
        echo "Update failed: /vpd/$keyword is value $(cat /vpd/$keyword)."
    fi

}

# Check if the /vpd directory is a mountpoint
if ! mountpoint -q "/vpd"; then
    echo "/vpd directory is not a mountpoint. Trying to mount /vpd..."
    # Try to mount /vpd
    mount -t squashfs -o offset=$((512*32768)) /dev/mmcblk0boot0 /vpd > /dev/null
    echo "mount /vpd..."
    # Check if the mount was successful
    if ! mountpoint -q "/vpd"; then
        echo "init the vpd partation"

        mkdir -p /var/vpd

        /usr/sbin/mksquashfs /var/vpd /var/vpd.img -no-duplicates -no-recovery > /dev/null
        # Set the device to read-only mode
        echo 0 > /sys/block/mmcblk0boot0/force_ro

        # Write vpd.img to /dev/mmcblk0boot0 at the specified offset
        dd if=/var/vpd.img of=/dev/mmcblk0boot0 seek=32768 > /dev/null 2>&1

        # Set the device back to read-write mode
        echo 1 > /sys/block/mmcblk0boot0/force_ro

        # Mount the new vpd image
        mount -t squashfs -o offset=$((512*32768)) /dev/mmcblk0boot0 /vpd > /dev/null
    fi
fi

# Create /var/vpd directory if it does not exist
mkdir -p /var/vpd

# Check if /vpd has files before copying
if [ "$(ls -A /vpd)" ]; then
    # Copy files from /vpd to /var/vpd
    cp -arf /vpd/* /var/vpd/
fi


# Delete the file if -d option is provided
if [ "$keyword" == "-d" ]; then
    rm -f "/var/vpd/$value"
    echo "File /var/vpd/$value has been deleted."
    update_vpd_partation  # Update the partition after deletion
    exit 0
fi


# Update value in /var/vpd
update_vpd_value
update_vpd_partation  # Update the partition
verify_vpd_value
