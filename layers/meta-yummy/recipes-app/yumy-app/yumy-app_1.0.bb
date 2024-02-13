SUMMARY = "Yumy App"
AUTHOR = "Jan Patrovsky<honzapatCZ@email.cz>"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS:append = " dotnet-native"

COMPATIBLE_HOST ?= "(x86_64|aarch64|arm).*-linux"

SRC_ARCH:aarch64 = "arm64"
SRC_ARCH:arm = "arm"
SRC_ARCH:x86-64 = "x64"

INSANE_SKIP:${PN} += "\
    already-stripped \
    staticdev \
"

SRC_URI = "git://github.com/RegiLikesMango/YumiUI.git"
SRCREV_default = "master"
PV = "1.0+git${SRCPV}"

RDEPENDS:${PN}:append = " \
    zlib \
    fontconfig \
"
#RDEPENDS:${PN}:append = " \
#     icu \
#     libgssapi-krb5 \
#"


S = "${WORKDIR}/git"
CSPROJ = "${S}/MyApp.csproj"

do_compile[network] = "1"

do_compile () {
    dotnet build ${CSPROJ} --output ${B}/${PN} --configuration release --runtime linux-${SRC_ARCH} --self-contained -p:PublishReadyToRun=true -p:PublishTrimmed=true     

    #FIXME: remove the following line. if the lttng-ust conflict is solved
    rm ${B}/${PN}/libcoreclrtraceptprovider.so
}

do_install () {
    install -d ${D}/opt/
    cp -r --no-preserve=ownership ${B}/${PN} ${D}/opt

    if [ "${SRC_ARCH}" = "x64" ]; then
        ln -s ${base_libdir} ${D}/lib64
    fi
}

FILES:${PN}:append = " /opt/${PN}/ /lib64"