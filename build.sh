#!/bin/bash
set -ex

binary_name="test"
obj_dir="./obj/"

mkdir -p "${obj_dir}"

cd ${obj_dir}
gcc -c ../list.c 
gcc -c ../ini.c 

gcc list.o ini.o -o ../${binary_name}



