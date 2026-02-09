#!/bin/bash 

set -e 

chmod +x bin/conv2d.run 

./bin/conv2d.run "$@"
