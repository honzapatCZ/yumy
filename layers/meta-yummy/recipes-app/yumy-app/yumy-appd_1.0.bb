inherit systemd

SUMMARY = "Yumy App Daemon"
AUTHOR = "Jan Patrovsky<honzapatCZ@email.cz>"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE:${PN} = "yumy-app.service"

SRC_URI:append = "file://yumy-app.service"
FILES:${PN}:append = " ${systemd_unitdir}/system/yumy-app.service" 

do_install:append() {
  install -d ${D}/${systemd_unitdir}/system
  install -m 0644 ${WORKDIR}/yumy-app.service ${D}/${systemd_unitdir}/system
}