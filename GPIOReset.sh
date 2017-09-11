#!/bin/bash
# Buttons on OLed board
echo 27 >/sys/class/gpio/unexport;# L_pin
echo 23 >/sys/class/gpio/unexport;# R_pin
echo  4 >/sys/class/gpio/unexport;#  C_pin
echo 17 >/sys/class/gpio/unexport;# U_pin
echo 22 >/sys/class/gpio/unexport;# D_pin
echo 5 >/sys/class/gpio/unexport;#  A_pin
echo 6 >/sys/class/gpio/unexport;#  B_pin
#
# Light sensor on PermaPro HAT
echo 21 >/sys/class/gpio/unexport;# Photo Transistor
