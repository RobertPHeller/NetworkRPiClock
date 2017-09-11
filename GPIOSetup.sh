#!/bin/bash
# Buttons on OLed board
echo 27 >/sys/class/gpio/export;# L_pin
echo in >/sys/class/gpio/gpio27/direction
echo 23 >/sys/class/gpio/export;# R_pin
echo in >/sys/class/gpio/gpio23/direction
echo  4 >/sys/class/gpio/export;#  C_pin
echo in >/sys/class/gpio/gpio4/direction
echo 17 >/sys/class/gpio/export;# U_pin
echo in >/sys/class/gpio/gpio17/direction
echo 22 >/sys/class/gpio/export;# D_pin
echo in >/sys/class/gpio/gpio22/direction
echo 5 >/sys/class/gpio/export;#  A_pin
echo in >/sys/class/gpio/gpio5/direction
echo 6 >/sys/class/gpio/export;#  B_pin
echo in >/sys/class/gpio/gpio6/direction
#
# Light sensor on PermaPro HAT
echo 21 >/sys/class/gpio/export;# Photo Transistor
echo in >/sys/class/gpio/gpio21/direction
