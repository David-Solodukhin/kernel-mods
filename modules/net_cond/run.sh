#!/bin/bash
make && sudo rmmod testmod && sudo insmod testmod.ko && dmesg

