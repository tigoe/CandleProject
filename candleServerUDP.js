/*
candleServerUDP.js

A combination web server, webSocket server, and TCP socket server in node.js.
Keeps track of clients in an array. Typing 'c' will print the array of clients

created 27 Jun 2015
modified 10 Jul 2015
by Tom Igoe
*/

var express = require('express');	// include express.js
io = require('socket.io'),				// include socket.io
dgram = require('dgram'),
getMac = require('getmac'),
app = express(),									// make an instance of express.js
webServer = app.listen(8000),			// start a web server with the express instance
webSocketServer = io(webServer);	// make a webSocket server using the express server

//  set up server and webSocketServer listener functions:
app.use(express.static('public'));					  // serve files from the public folder
app.get('/:name', serveFiles);							  // listener for all static file requests
webSocketServer.on('connection', openSocket);	// listener for websocket data

var clients = new Array,  // array to track TCP clients when they connect
input = '';               // input string from the keyboard (STDIN)
//var tcpServer = net.createServer(listenForClients);  // create the server
//tcpServer.listen(8080);           // start the TCP socket server listening
var udpServer = dgram.createSocket('udp4');
udpServer.bind(8888);

var stdin = process.openStdin();    // enable input from the keyboard
stdin.setEncoding('utf8');          // encode everything typed as a string

// serve web files from /public directory:
function serveFiles(request, response) {
  var fileName = request.params.name;				// get the file name from the request
  response.sendFile(fileName);							// send the file
}

// open a webSocket in response to a client request:
function openSocket(webSocket){
  console.log('new user address: ' + webSocket.handshake.address);
  // send something to the web client with the data:
  webSocket.emit('message', 'Hello, ' + webSocket.handshake.address);
  // this function runs if there's input from the web client:
  webSocket.on('message', function(data) {
    // doing something here
    console.log(data);
    broadcast(data);
  });

  webSocket.on('disconnect', function() {
    // doing something here
    console.log('webSocket client disconnected');
  });
}
// this function runs if there's input from the keyboard.
// you need to hit enter to generate this event.
stdin.on('data', function(data) {
  data = data.trim();                 // trim any whitespace from the string
  if (data === 'c') {
    console.log(clients);             // list the client array
  } else {
    broadcast(data);                  // otherwise, broadcast the message to all clients
  }
});


udpServer.on('listening', function () {
  var address = udpServer.address();
  console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

udpServer.on('message', function (message, remote) {
  console.log(new Date() + " " + remote.address + ':' + remote.port +' - ' + message);
  if (getMac.isMac(message)) {
    console.log('client sent a MAC address: ' + message);
    checkForNewClient(remote.address, 8888, message);
    sendPacket(remote.address, 8888, 'Hello!');
  } else {
    broadcast(message);
  }
});


function sendPacket(address, port, data) {
  var client = dgram.createSocket('udp4');
  client.send(data, 0, data.length, port, address, function(error, bytes) {
    if (error) throw error;
    //console.log('UDP message sent to ' + address +':'+ port);
    client.close();
  });
}

function checkForNewClient(ip, port, mac) {
  var isNewClient = true;
  // make a new JSON object with this data:
  var newClient = {
    'address': ip,
    'port': port,
    'macAddress' : mac
  };
  // see if the client IP address matches one in the list:
  for (thisClient in clients) {
    if (clients[thisClient].address === newClient.address ) {
      isNewClient = false;
    }
  }
  // if this is a new client, add it to the client list:
  if (isNewClient) {
    clients.push(newClient);
  }
}

// this function broadcasts data to all UDP clients.
function broadcast(data) {
  for (thisClient in clients) {     // iterate over the client array
    sendPacket(clients[thisClient].address, clients[thisClient].port, data);
  }
}
