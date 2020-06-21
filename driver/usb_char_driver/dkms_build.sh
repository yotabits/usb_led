#!/bin/bash
# delete old files
rm -rf dkms_tree
rm -rf source_tree
# create necessary directories
mkdir dkms_tree
mkdir source_tree

source_dir="usb_rgb"
version=$(grep -o "[^PACKAGE_VERSION=]*\..*$" $source_dir/dkms.conf)

cp -r $source_dir source_tree
mv source_tree/$source_dir source_tree/$(echo $source_dir)-$version
actual_path=$(pwd)

/usr/sbin/dkms build add --dkmstree $actual_path/dkms_tree --sourcetree $actual_path/source_tree -m $source_dir -v $version
/usr/sbin/dkms build --dkmstree $actual_path/dkms_tree --sourcetree $actual_path/source_tree -m $source_dir -v $version
/usr/sbin/dkms mkdeb --dkmstree $actual_path/dkms_tree --sourcetree $actual_path/source_tree -m $source_dir -v $version --source-only

cp dkms_tree/$source_dir/$version/deb/*.deb .
