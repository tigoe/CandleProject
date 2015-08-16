/*
candleServerUDP.js

A combination web server and UDP socket server in node.js.

created 27 Jun 2015
modified 29 Jul 2015
by Tom Igoe
*/

var express = require('express'),	    // include express.js
  dgram = require('dgram'),           // include UDP datagram functions
  getMac = require('getmac'),         // include getmac
  app = express();							      // make an instance of express.js

// start a web server with the express instance:
app.listen(8000, function() {
  console.log(new Date() + '\tweb server listening on port 8000');
});

app.use(express.static('public'));	  // serve files from the public folder
app.get('/files/:name', serveFiles);  // route for all static file requests
app.get('/clients/', listClients);    // route for client list request
app.get('/send/:action/:target', remoteCommand);  // route for all remote actions

var clients = new Array,              // array to track UDP clients when they connect
  UDP_PORT = 8888;                    // all UDP transactions will be on 8888

var udpServer = dgram.createSocket('udp4'); // create UDP socket
udpServer.bind(UDP_PORT);                   // bind the socket server to a port number

var stdin = process.openStdin(),      // enable input from the keyboard
input = '';                           // input string from the keyboard (STDIN)
stdin.setEncoding('utf8');            // encode everything typed as a string

// serve web files from /public directory:
function serveFiles(request, response) {
  var fileName = request.params.name;	 // get the file name from the request
  var options = {							         // options for serving files
    root: __dirname + '/public/'		   // root is the /public directory in the app directory
  };
  response.sendFile(fileName, options); // send the file
}

// send the UDP client list to a web client:
function listClients(request, response){
  response.send(clients);
  response.end();
}

// send a web client's command to one or all UDP clients.
// request looks like this: /send/:action/:target
function remoteCommand(request, response){
  console.log(new Date() + '\tweb request: send '
      + request.params.action
      + ' to ' + request.params.target);

  var message;        // message for UDP clients
  switch(request.params.action) {
    case 'login':     // login generates !!!
    message = "!!!";
    break;
    case 'logout':    // logout generates ~~~
    message = "~~~";
    break;
    case 'burst':     // burst generates *
    message = "*";
    break;
  }

  // if the web client sends 'all' as the target, broadcast to all UDP clients:
  if (request.params.target === 'all') {
    broadcast(message);
    // if the web client sends an IP address as the target, send to that address:
  } else {
    sendPacket(request.params.target, message);
  }

  // reply to web client:
  var content = 'message: ' + message + ' sent to: ' + request.params.target;
  response.send(content);
  response.end();
}

// this function runs if there's input from the keyboard.
// you need to hit enter to generate this event.
stdin.on('data', function(data) {
  data = data.trim();     // trim any whitespace from the string
  switch (data) {
    case 'c':
    console.log(new Date() + '\t' + JSON.stringify(clients, null, 2)); // list the client array
    break;
    default:
    sendAll(data);        // send the message to all clients
    break;
  }
});

// event handlers for UDP server:

// runs when UDP server is up and running:
udpServer.on('listening', function () {
  var address = udpServer.address();
  udpServer.setBroadcast(true);
  console.log(new Date() + '\tUDP Server listening on ' + address.address + ":" + address.port);
});

// runs when a UDP packet arrives:
udpServer.on('message', function (message, remote) {
  console.log(new Date() + '\t' + remote.address + ':' + remote.port +'\t' + message);
  if (getMac.isMac(message)) {
    // convert byte array to string:
    messageString = String.fromCharCode.apply(null, new Uint8Array(message));
    messageString = messageString.trim();
    checkForNewClient(remote.address, messageString);
    sendPacket(remote.address, 'Hello!!!');
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
      console.log(new Date() + '\terror: ' + error);
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
    if (clients[thisClient].address === newClient.address ) {
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
    interval = (Date.parse(now)/1000) - (Date.parse(clientTime)/1000);
    console.log("cleanup:  interval = " + interval );
    if (interval > 600) { // ten minutes
      // send a logout:
      sendPacket(clients[thisClient].address, UDP_PORT, '~~~');
      // remove the client from the array:
      clients.splice(thisClient, 1);
    }
  }
}

function logMessageTime(thisAddress) {
  for (thisClient in clients) {
    if (clients[thisClient].address === thisAddress ) {
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
  console.log('broadcast');
   for (b=3; b<20; b++){
     sendPacket('192.168.1.' + b, data);
   }
}
