/*
candleServer.js

A combination web server, webSocket server, and TCP socket server in node.js.
Keeps track of clients in an array. Typing 'c' will print the array of clients

created 27 Jun 2015
modified 10 Jul 2015
by Tom Igoe
*/

var express = require('express');	// include express.js
io = require('socket.io'),				// include socket.io
net = require('net'),             // include the net library
app = express(),									// make an instance of express.js
webServer = app.listen(8000),			// start a web server with the express instance
webSocketServer = io(webServer);	// make a webSocket server using the express server

//  set up server and webSocketServer listener functions:
app.use(express.static('public'));					  // serve files from the public folder
app.get('/:name', serveFiles);							  // listener for all static file requests
webSocketServer.on('connection', openSocket);	// listener for websocket data

process.setMaxListeners(0);     // prevent memory leak when clients are messy

var clients = new Array,  // array to track TCP clients when they connect
input = '';               // input string from the keyboard (STDIN)
var tcpServer = net.createServer(listenForClients);  // create the server
tcpServer.listen(8080);           // start the TCP socket server listening

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

  var tcpConnection = new net.Socket();			// make a new instance of the TCP socket class
  tcpConnection.connect(8080, 'localhost', login);	// connect to the TCP server

  // this function runs if there's input from the web client:
  webSocket.on('message', function(data) {
    // doing something here
    console.log(data);
    broadcast(data);
  });

  webSocket.on('disconnect', function() {
    // doing something here
    console.log('webSocket client disconnected');
    tcpConnection.end();
  });


  // this function runs when the TCP client successfully connects:
  function login() {
    tcpConnection.setEncoding('utf8');	// encode everything sent by the client as a string
    console.log('Connected');
    tcpConnection.write('Hello ' + tcpConnection.localAddress);	// send a hello and your address

    // this function runs if the TCP server sends data:
    tcpConnection.on('data', function(data) {
      data = data.trim();							      // trim any whitespace from the string
      webSocket.emit('message', data);      // send the TCP client data to the WS client
      console.log('Server said: ' + data);	// print the data
    });

    // this function runs if the client ends, either locally
    // or from the server:
    tcpConnection.on('end', function() {
      console.log("socket closed");
    });
  }
}

// This function is called every time a new TCP client connects:
function listenForClients(tcpClient) {
  console.log('client connected at ' + new Date());
  tcpClient.setEncoding('utf8');  // encode everything sent by the client as a string
  tcpClient.write('hello');      // send the client a hello message
  clients.push(tcpClient);       // append the client to the array of clients

  // this function runs if the client sends an 'end' event:
  tcpClient.on('end', function() {
    console.log('client disconnected at ' + new Date() );
    var position = clients.indexOf(tcpClient); // get the client's position in the array
    clients.splice(position, 1);            // delete it from the array
  });

  // this function runs if the client sends data:
  tcpClient.on('data', function(data) {
    data = data.trim();            // trim any whitespace from the string
    console.log(data);
    broadcast(data);
    if (data === 'x') {            // if the client sends 'exit',
    console.log('closing client');
    tcpClient.end();                  // disconnect the client
  });

  // handle any network errors:
  tcpClient.on('error', function (err) {
    console.error('Network connection error', err);
    console.error('client: ' + tcpClient.remoteAddress);
    tcpClient.end();
  });


  function checkClientList() {
    console.log('cleaning up client list');
    for (c in clients) {
      if (typeof(client[c]) === 'undefined') {
        var position = clients.indexOf(client); // get the client's position in the array
        clients.splice(position, 1);            // delete it from the array
      }
    }
  }

  setInterval(checkClientList, 5000);
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

// this function broadcasts data to all TCP clients.
function broadcast(data) {
  for (thisClient in clients) {     // iterate over the client array
    clients[thisClient].write(data, function (error){
      if (error) {
        console.error(error);
        clients[thisClient].end();
      }
    });// send the message to each client
  }
}
