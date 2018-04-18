#!/usr/bin/python

import bluetooth
import ax25

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
			ax25.main()
		elif data == "!BEX":
			print "Bluetooth Terminated"
			break
		elif data == "!EX":
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
