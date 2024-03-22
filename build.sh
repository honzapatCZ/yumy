#!/bin/bash

# Set default value for MACHINE environment variable
export MACHINE="yummy"

# Parse command-line arguments
while [[ $# -gt 0 ]]
do
    key="$1"
    case $key in
        -e|--emulator)
            export MACHINE="qemuarm64"
            shift
            ;;
        -u|--update)
            TARGET="update"
            shift
            ;;
        -i|--image)
            TARGET="image"
            shift
            ;;
        -s|--sdk)
            TARGET="sdk"
            shift
            ;;
        -g|--graph)
            TARGET="graph"
            shift
            ;;
        -menu)
            TARGET="menu-config"
            shift
            ;;
        -mk)
            TARGET="modify-kernel"
            shift
            ;;
        -fk)
            TARGET="finish-kernel"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done


# Set OE environment by sourcing oe-init script
source layers/poky/oe-init-build-env

# Build target based on command-line argument
case $TARGET in
    "image")
        bitbake core-image-minimal
        ;;
    "update")
        bitbake core-image-minimal
        bitbake update-bundle
        ;;
    "sdk")
        bitbake core-image-minimal -c populate_sdk
        ;;
    "graph")
        bitbake -g -I u-boot -I linux-mainline -I uboot core-image-minimal
        dot -Tpng task-depends.dot -o task-depends.png
        ;;
    "menu-config")
        bitbake -c menuconfig linux-mainline
        bitbake -c diffconfig linux-mainline
        ;;
    "modify-kernel")
        devtool modify linux-mainline
        ;;
    "finish-kernel")
        devtool finish linux-mainline ../layers/meta-yummy
        ;;
    *)
        echo "Unknown target: $TARGET"
        exit 1
        ;;
esac