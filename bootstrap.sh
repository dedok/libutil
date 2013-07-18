#!/bin/bash

REPO_ROOT=$(pwd)
BUILD_PATH=$REPO_ROOT/tmp-build
BUILD_TYPE=RelWithDebugInfo

die() {
    echo "ERROR : $@" 1>&2 && exit 1
}

run_cmake() {
    local ex_args=
    cmake -H$REPO_ROOT -B$BUILD_PATH -DCMAKE_BUILD_TYPE=$BUILD_TYPE $ex_args
}

usage() {
    cat << EOF
usage: bootstrap.sh [--help|-h][--debug|-d][--rdebug|-rd][--release|-r]
EOF
}

[ -d $BUILD_PATH ] && rm -r $BUILD_PATH
mkdir $BUILD_PATH

for option; do
    case $option in
        --help    | -h)     usage && exit 0               ;;
        --debug   | -d)     BUILD_TYPE=Debug              ;;
        --rdebug  | -rd)    BUILD_TYPE=RelWithDebugInfo   ;;
        --release | -r)     BUILD_TYPE=Release            ;;
        * )
            usage && \
                die "bootstrap failed, unknown command line parameter '$option', use --help|-h for help"
        ;;
    esac
done

run_cmake

