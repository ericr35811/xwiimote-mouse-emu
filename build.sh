#!/bin/bash
set -ex

binary_name="test"
obj_dir="./obj/"

mkdir -p "${obj_dir}"

evdev_flags=$(pkg-config --libs --cflags libevdev)

cd ${obj_dir}
gcc -c ../list.c 
gcc -c ../ini.c 
gcc -c ../evdev_helpers.c $evdev_flags
gcc -c ../uinput.c        $evdev_flags
gcc -c ../main.c          $evdev_flags

gcc *.o -o ../${binary_name} $evdev_flags



