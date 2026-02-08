#!/bin/bash

set -e

./01-build.sh
chmod +x bin/conv2d.run

./bin/conv2d.run "$@"
