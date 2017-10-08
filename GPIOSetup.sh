#!/bin/bash
# Buttons on OLed board -- needs wiringPi installed
gpio mode 2 up
gpio export 27 in;# L_pin
gpio mode 4 up
gpio export 23 in;# R_pin
gpio mode 7 up
gpio export 4 in;#  C_pin
gpio mode 0 up
gpio export 17 in;# U_pin
gpio mode 3 up
gpio export 22 in;# D_pin
gpio mode 21 up
gpio export 5 in;#  A_pin
gpio mode 22 up
gpio export 6 in;#  B_pin
#
# Light sensor on PermaPro HAT
gpio mode 29 up
gpio export 21 in;# Photo Transistor
