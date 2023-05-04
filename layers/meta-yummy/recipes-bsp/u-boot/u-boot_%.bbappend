FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://sun8i-v3s-yummy.dts;subdir=git/arch/arm/dts \
            file://Yummy_defconfig;subdir=git/configs/"