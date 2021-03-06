#!/usr/bin/python

import bluetooth
import sys
import time
import pexpect
import subprocess
import signal

def debug(socket, debugmsg):
	print debugmsg
	socket.send(debugmsg)

while 1:
	server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
	port = 1
	server_socket.bind(("",port))
	server_socket.listen(1)

	client_socket,address = server_socket.accept()
	print "Accepted connection from ", address

	while 1:
		try:
			data = client_socket.recv(1024)
			if data == "!AX":
				debug(client_socket, "Initiating connection to server\n")
				ax25 = pexpect.spawn("call -r -S 1 YD0SHY-2", echo = False)
				ax25.expect('Rawmode')
				debug(client_socket, "Connection initiated, sending test message\n")
				message = 'Uji coba mengirimkan pesan menggunakan INA-Rad dengan protokol AX.25'
				ax25.sendline(message)
				time.sleep(5)
				ax25.kill(signal.SIGKILL)
				time.sleep(5)
				debug(client_socket, "Connection terminated\n")
			elif data == "!AXAX":
				debug(client_socket, "Initiating connection to server YD0SHY-3\n")
				ax25_yd0shy3 = pexpect.spawn("call -r -S 1 YD0SHY-3", echo = False)
				ax25_yd0shy3.expect('Rawmode')
				debug(client_socket, "Connection initiated, sending test message\n")
				message = 'Uji coba mengirimkan pesan menggunakan INA-Rad dengan protokol AX.25'
				ax25_yd0shy3.sendline(message)
				time.sleep(5)
				ax25.kill(signal.SIGKILL)
				time.sleep(5)
				debug(client_socket, "Connection terminated\n")
			elif data == "!BEX":
				debug(client_socket, "Bluetooth connection terminated\n")
				print "Bluetooth Terminated"
				break
			elif data == "!EX":
				client_socket.send("Server terminated" + "\n");
				break
			else:
				print "Received data = [%s]" % data
		except:
			print "Connection error\n"
			break;
	if data == "!EX":
		break
	else:
		client_socket.close()
		server_socket.close()

client_socket.close()
server_socket.close()
