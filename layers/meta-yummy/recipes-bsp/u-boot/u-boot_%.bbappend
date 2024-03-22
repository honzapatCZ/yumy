FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

#SRC_URI += "file://sun8i-v3s-yummy.dts;subdir=git/arch/arm/dts"

SRC_URI += "file://sun8i-v3s-yummy.dts;subdir=git/arch/arm/dts"

do_configure:prepend() {
    # Remove exisiting fdtfile, if there is one
    sed -i '/CONFIG_DEFAULT_DEVICE_TREE="sun8i-v3s-licheepi-zero"/d' ${S}/configs/LicheePi_Zero_defconfig
    #sed -i 's/CONFIG_DEFAULT_DEVICE_TREE="sun8i-v3s-licheepi-zero"/CONFIG_DEFAULT_DEVICE_TREE="sun8i-v3s-yummy"/' ${S}/configs/LicheePi_Zero_defconfig

    # Add new fdtfile
    sed -i '$a\CONFIG_DEFAULT_DEVICE_TREE="sun8i-v3s-yummy"' ${S}/configs/LicheePi_Zero_defconfig
    sed -i '$a\CONFIG_EXTRA_ENV_SETTINGS="fdt_file=sun8i-v3s-yummy.dtb\\0"' ${S}/configs/LicheePi_Zero_defconfig
}
