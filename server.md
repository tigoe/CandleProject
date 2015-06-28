# Server protocol

Clients connect on port 8080

Client properties:

* IP address (not guaranteed consistent)
* MAC address (unique)
* Table number (assigned by server)

On connect:

* client sends ??
* Server sends ?? 

On client touch event:

* Client sends event + time to live
* In response, server sends event + TTL-1 to client's nearest neighbors
* If TTL > 0, client sends touch event to server
* recurse



