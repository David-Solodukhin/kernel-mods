#!/bin/bash
LASTLINE=$(dmesg | tail -1);
CURRLINE="TEST";
make;
lsmod | grep 'testmod' > /dev/null && sudo rmmod testmod;
sudo insmod testmod.ko
echo PRESS q TO UNLOAD ----------------------------------
while true; do
CURRLINE=$(dmesg | tail -1);
if [[ "$CURRLINE" != "$LASTLINE" ]]; 
then
dmesg | tail -1;
LASTLINE="$CURRLINE";
 read -t 0.25 -N 1 input
    if [[ $input = "q" ]] || [[ $input = "Q" ]]; then
  	sudo rmmod testmod
  	exit 1;
    fi
fi
done;
