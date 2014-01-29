#!/bin/bash
cp BB-SPI1-01-00A0.dtbo /lib/firmware
echo BB-SPI1-01 > /sys/devices/bone_capemgr.*/slots
