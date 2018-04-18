#!/usr/bin/python

import bluetooth
import sys
import time
import pexpect
import subprocess
import signal

while 1:
	server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
	port = 1
	server_socket.bind(("",port))
	server_socket.listen(1)

	client_socket,address = server_socket.accept()
	print "Accepted connection from ", address

	while 1:
		data = client_socket.recv(1024)
		if data == "!AX":
			client_socket.send("Initiating connection to server" + "\n")
			ax25 = pexpect.spawn("call -r -S 1 YD0SHY-2", echo = False)
			ax25.expect('Rawmode')
			client_socket.send("Connection initiated, sending test message" + "\n")
			message = 'Uji coba mengirimkan pesan menggunakan INA-Rad dengan protokol AX.25'
			ax25.sendline(message)
			time.sleep(10)
			ax25.kill(signal.SIGKILL)
			client_socket.send("Connection terminated" + "\n")
		elif data == "!BEX":
			client_socket.send("Bluetooth connection terminated" + "\n")
			print "Bluetooth Terminated"
			break
		elif data == "!EX":
			client_socket.send("Server terminated" + "\n");
			break
		else:
			print "Received data = [%s]" % data
	if data == "!EX":
		break
	else:
		client_socket.close()
		server_socket.close()

client_socket.close()
server_socket.close()
