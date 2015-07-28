# udpBridge.py
import socket
import time
from bridgeclient import BridgeClient

serverAddress = "192.168.1.2"
serverPort = 8888
bridge = BridgeClient()
inSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

inSocket.bind(("",8888))

while True:
    try:
        incoming = inSocket.recv(255)
        bridge.put("in", incoming)
    except KeyboardInterrupt:
        break
    except Exception as err:
        print err
        pass
