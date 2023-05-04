FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PACKAGE_ARCH = "${MACHINE_ARCH}"

SRC_URI:append = " file://usb.cfg"
SRC_URI:append = " file://wifi.cfg"

SRC_URI += "file://sun8i-v3s-yummy.dts;subdir=linux-6.1.9/arch/${ARCH}/boot/dts"

# make dt available for kernel build things.
KERNEL_DEVICETREE:append = " sun8i-v3s-yummy.dtb "
