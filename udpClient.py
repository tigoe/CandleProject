# udpSender.py
import sys
import socket
import time
serverAddress = sys.argv[1] #"192.168.1.2"
serverPort = 8888

outgoing =  sys.argv[2]

outSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
outSocket.connect((serverAddress, serverPort))
outSocket.send(outgoing)
outSocket.close()
