#!/bin/bash 

set -e

make clean
make 
echo 
ls -l bin/*.run
