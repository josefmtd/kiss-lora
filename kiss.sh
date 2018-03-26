#!/bin/sh
#how to use: sudo ./kiss.sh <tty port> <ax25 port>
#start AX25 using port defined in /etc/ax25/axports, for me it's lora1
/usr/sbin/kissattach -m 231 $1 $2
sleep 1
#Set KISS mode to NO CRC
/usr/sbin/kissparms -p $2 -c 1
sleep 1
#Set AX25 Parameter
echo $3 > /proc/sys/net/ax25/ax0/standard_window_size
echo 231 > /proc/sys/net/ax25/ax0/maximum_packet_length
sleep 1
#Added FM Radio
#/usr/sbin/kissattach -m 231 $4 $5
#Set AX25 Daemon as configured in /etc/ax25/ax25d.conf
/usr/sbin/ax25d
sleep 1
#Setup MHeard Daemon
/usr/sbin/mheardd
#end of script
