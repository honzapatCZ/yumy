SUMMARY = "U-boot bootloader fw_env provider"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SECTION = "bootloader"
PROVIDES = "u-boot-fwenv"

SRC_URI += "file://fw_env.config"

inherit uboot-config


do_install () {
    #install -m 755 ${S}/tools/env/fw_printenv ${D}${base_sbindir}/fw_printenv
    #ln -s fw_printenv ${D}${base_sbindir}/fw_setenv
    install -d ${D}${sysconfdir}/
    install -m 644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
