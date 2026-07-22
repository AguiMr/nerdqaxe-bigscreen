#!/bin/bash

rpath="$( dirname "$( readlink -f "$0" )" )"
cd $rpath

docker run --rm -it -v /dev:/dev --privileged  -e BOARD="${BOARD:-NERDQAXEPLUS2}" -e BIGSCREEN="${BIGSCREEN:-}" -v "$rpath/..":/home/builder/project esp-idf-builder idf.py $@
