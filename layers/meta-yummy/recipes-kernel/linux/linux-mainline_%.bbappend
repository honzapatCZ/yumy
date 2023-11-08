FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PACKAGE_ARCH = "${MACHINE_ARCH}"

#SRC_URI:append = " file://usb.cfg"
SRC_URI:append = " file://wifi.cfg"
SRC_URI:append = " file://crypto.cfg"
SRC_URI:append = " file://display.cfg"

SRC_URI += "file://sun8i-v3s-yummy.dts;subdir=linux-6.1.9/arch/${ARCH}/boot/dts"
SRC_URI += "file://st7796.c;subdir=linux-6.1.9/drivers/gpu/drm/tiny"
SRC_URI += "file://ns2009.c;subdir=linux-6.1.9/drivers/input/touchscreen"
SRC_URI += "file://0001-display-and-touchscreen.patch"

# make dt available for kernel build things.
#KERNEL_DEVICETREE:append = " sun8i-v3s-yummy.dtb "
