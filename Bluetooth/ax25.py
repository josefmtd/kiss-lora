import sys
import time
import pexpect
import subprocess
import signal


def main():
	ax25 = pexpect.spawn("call -r -S 1 YD0SHY-2", echo = False)
	ax25.expect('Rawmode')
	message = 'Uji coba mengirimkan pesan menggunakan INA-Rad dengan protokol AX.25'
	ax25.sendline(message)
	time.sleep(10)
	ax25.kill(signal.SIGKILL)

main()
