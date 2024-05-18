SUMMARY = "Simple LED driver for BeagleBone Green"
DESCRIPTION = "Allows setting pins high/low"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


TARGET_CC_ARCH += "${LDFLAGS}"

inherit module

SRC_URI = "file://new-led-dibdriver.c file://Makefile"

S = "${WORKDIR}"
