# kiss-lora
An attempt to re-create a KISS TNC algorithm in LoRa using Arduino

## Arduino Sketch

This Arduino sketch is programmed to work with LoRa SX1278 based implementation. This Arduino sketch uses RadioHead Library from https://airspayce.com/mikem/arduino/RadioHead and use the following settings for the LoRa:

- ModemConfig: Bw125Cr45Sf128
- PreambleLength: 8 (bytes)
- Frequency: 434.0 (MHz)
- TxPower: 13 (dBm)

This sketch is a non CRC KISS frame implementation based on https://github.com/flok99/arduino-kiss, I did some cleaning to make this program more readable. I modified some of the functions inside the OOP Class: KISS to work with KISS frame without CRC. 

This sketch includes a KISS Library, this Library consists of KISS Class. KISS Class is kept generic so that this KISS TNC algorithm can be ported to another microcontroller should the project reach a dead end with an Arduino implementation. This implementation requires CallBack functions in the Sketch to do the Low-Level Programming of Serial Comm. via SPI or UART.

## How to Use with Raspberry Pi

The goal of this KISS TNC is to be able to use LoRa as the Physical Layer of an AX.25 Radio Network. I have created a simple shell to execute commands to setup your KISS TNC on Raspberry Pi.

```
#!/bin/sh
#start AX25 using port defined in /etc/ax25/axports, for me it's lora1
/usr/sbin/kissattach -m 232 $1 $2
sleep 1
#Set KISS mode to NO CRC
/usr/sbin/kissparms -p $2 -c 1
sleep 1
#Set AX25 Parameter
echo 1 > /proc/sys/net/ax25/ax0/standard_window_size
echo 231 > /proc/sys/net/ax25/ax0/maximum_packet_length
sleep 1
#Set AX25 Daemon as configured in /etc/ax25/ax25d.conf
/usr/sbin/ax25d
sleep 1
#Setup MHeard Daemon
/usr/sbin/mheardd
#end of script
```

Before running the shell, we need to make sure to set our KISS TNC in the config files:

```
# nano /etc/ax25/axports
```

In which you will insert a line that goes something like this:

```
# /etc/ax25/axports
#
# The format of this file is:
#
# name callsign speed paclen window description
#
lora1   YD0SHY-0        9600    231     1       LoRa KISS_NO_CRC
```

1. Portname can be any characters you like, for example, mine is "lora1"
2. Callsign is your amateur radio callsign, for LoRa, you can do any 6 Alphanumeric characters separated by a "-" with a number from 0-15, for example, mine is "YD0SHY-0"
3. Speed is the speed of the modem you use, for this modem it is running on 9600 baud
Paclen is the maximum packet length the modem can handle, for example, mine is 231. There's a reason the packet length is 231, since LoRa has 256 bytes buffer, 1 byte is used for length, 4 bytes are used for RadioHead addressing, 1 byte for KISS Command Byte, 21 bytes are used for AX.25 addressing (assuming 14 bytes (source + destination) and 7 bytes (digipeater), and 2 bytes for Control and PID bytes. This leads to only 256-(1+1+4+21+2) which is 227, however I put it on 231 since I am planning to re-write the RadioHead so that the 4 bytes LoRa addressing would no longer be needed.
4. Window is the maximum outstanding packet can be sent without acknowledge from the receiving end, this is done to prevent KISS TNC from receiving more than one packet consecutively.

Next what you need to do is add configuration files to AX.25 Daemon so that the Daemon can handle call request to your domain. To access the AX.25 Daemon Config, what you need to do is:

```
# nano /etc/ax25/ax25d.conf
```

In which you will insert a line that goes something like this:

```
[YD0SHY-0 VIA lora1]
NOCALL   * * * * * *    L
default  * * * * * *    -       root /usr/sbin/ax25-node ax25-node
```

What the above configuration file means is that when a node calls for YD0SHY-1 on port lora1 it will answer by executing the program /usr/sbin/ax25-node ax25-node, the asterisks before indicate the AX.25 parameters you wish to use when running the program, the asterisks representing a "set as a default mode", so it will follow any configurations you have set on AX.25 ports, you can see the default at /proc/sys/net/ax25/ax0 (ax0 can be ax1, etc. if you have more than one AX.25 port).

For example on how you can use this to access higher level programs like XMPP you can go check out dmahony's Installation Instruction on https://github.com/dmahony/Lora-Chat-Device/ and also refer to the program he uses which can be accessed in https://github.com/jgoerzen/ax25xmpp

For questions regarding this project please contact me on josstemat@gmail.com
