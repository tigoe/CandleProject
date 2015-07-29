/*
candleServerUDP.js

A combination web server  and UDP socket server in node.js.

created 27 Jun 2015
modified 29 Jul 2015
by Tom Igoe
*/

var express = require('express');	// include express.js
dgram = require('dgram'),
getMac = require('getmac'),
app = express(),									// make an instance of express.js
webServer = app.listen(8000),			// start a web server with the express instance

//  set up server and webSocketServer listener functions:
app.use(express.static('public'));					  // serve files from the public folder
app.get('/files/:name', serveFiles);							  // listener for all static file requests
// need to define other routes here:
app.get('/login/', loginAll);
app.get('/logout/', logoutAll);
app.get('/clients/', listClients);
app.get('/burst/', burstAll);

//    list current clients
//    send message to a client
//    send a broadcast message
//    check a given client's data

var clients = new Array,  // array to track TCP clients when they connect
UDP_PORT = 8888,           // all UDP transactions will be on 8888
input = '';               // input string from the keyboard (STDIN)

var udpServer = dgram.createSocket('udp4');
udpServer.bind(UDP_PORT, function() {
  udpServer.setBroadcast(true);
});

var stdin = process.openStdin();    // enable input from the keyboard
stdin.setEncoding('utf8');          // encode everything typed as a string

// serve web files from /public directory:
function serveFiles(request, response) {
  var fileName = request.params.name;				// get the file name from the request
  var options = {							// options for serving files
    root: __dirname + '/public/'		// root is the /public directory in the app directory
  };
  // if there's an error sending a file, this function
  // will be called:
  function fileError(error) {
    if (error) {								// if there's an error
    response.status(error.status)		// and send a HTTP 404 status message
    response.end();						// and close the connection
  }

  response.sendFile(fileName, options, fileError);							// send the file
}
}

function listClients(request, response){
  var content = clients;
  response.send(content);
  response.end();
}

function loginAll(request, response){
  var content = "logged in all";
  broadcast('!!!');
  response.send(content);
  response.end();
}

function logoutAll(request, response){
  var content = "logged out all";
  broadcast('~~~');
  response.send(content);
  response.end();
}

function burstAll(request, response){
  var content = "burst all";
  broadcast('*');
  response.send(content);
  response.end();
}



// this function runs if there's input from the keyboard.
// you need to hit enter to generate this event.
stdin.on('data', function(data) {
  data = data.trim();       // trim any whitespace from the string
  switch (data) {
    case 'c':
    console.log(clients); // list the client array
    break;
    case '~':
    broadcast('~~~');
    break;
    case '!':
    broadcast('!!!');
    break;
    default:
    sendAll(data);        // send the message to all clients
    break;
  }
});

udpServer.on('listening', function () {
  var address = udpServer.address();
  console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

udpServer.on('message', function (message, remote) {
  console.log(new Date() + " " + remote.address + ':' + remote.port +' - ' + message);

  if (getMac.isMac(message)) {
    console.log('Client sent login message');
    // convert byte array to string:
    messageString = String.fromCharCode.apply(null, new Uint8Array(message));
    checkForNewClient(remote.address, messageString);
    sendPacket(remote.address, 'Hello!');
  } else {
    sendAll(message);
  }
  // log this time as the last message time from this client:
  logMessageTime(remote.address);
});

function sendPacket(address, data) {
  var client = dgram.createSocket('udp4');
  client.send(data, 0, data.length, UDP_PORT, address, function(error, bytes) {
    if (error) {
      //throw error;
      console.log("error: " + error);
    }
    client.close();
  });
}

function checkForNewClient(ip, mac) {
  var isNewClient = true;
  // make a new JSON object with this data:
  var newClient = {
    'address': ip,
    'macAddress' : mac
  };
  // see if the client IP address matches one in the list:
  for (thisClient in clients) {
    if (clients[thisClient].ip === newClient.ip ) {
      isNewClient = false;
    }
  }
  // if this is a new client, add it to the client list:
  if (isNewClient) {
    clients.push(newClient);
  }
}

function cleanClientList() {
  var now = new Date();
  var interval, clientTime;
  for (thisClient in clients) {
    clientTime = clients[thisClient].lastMessageTime;
    interval = Date.parse(now)/1000 - Date.parse(clientTime)/1000;
    if (interval > 600) { // ten minutes
      // send a logout:
      sendPacket(clients[thisClient].address, UDP_PORT, '~~~');
      // remove the client from the array:
      clients.splice(thisClient, 1);            // delete it from the array
    }
  }
}

function logMessageTime(thisAddress) {
  for (thisClient in clients) {
    if (clients[thisClient].ip === thisAddress ) {
      clients[thisClient].lastMessageTime = new Date();
    }
  }
}

// this function broadcasts data to all UDP clients.
function sendAll(data) {
  for (thisClient in clients) {     // iterate over the client array
    sendPacket(clients[thisClient].address, data);
  }
}

function broadcast(data) {
  sendPacket('192.168.0.255', data);
}
