inherit systemd

SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE_${PN} = "yumy-app.service"

SRC_URI:append = "file://yumy-app.service"
FILES:${PN}:append = " ${systemd_unitdir}/system/yumy-app.service" 

do_install_append() {
  install -d ${D}/${systemd_unitdir}/system
  install -m 0644 ${WORKDIR}/yumy-app.service ${D}/${systemd_unitdir}/system
}