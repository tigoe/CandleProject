var socket = io();		      // socket.io instance. Connects back to the server


var clients = [];
var clientCount = 24;
var tableSize = 30;
var img, img2;
function preload() {
  img = loadImage('img/circle.svg');
  img2 = loadImage('img/redcircle.svg');
}

function setup() {
  image(img, 30, 30);
  image(img2, 30, 30);
  var width = 8;
  createCanvas(800, 600);
  for (var c=0; c<clientCount; c++) {
    var row = ((c % width) * (tableSize + 5)) + tableSize;
    var col = (Math.floor(c / width)* (tableSize+ 5)) + tableSize;
    var myLoc = {'x': row, 'y': col };

    var table = createSprite(row, col);
    table.mouseIsActive = true;
    table.addImage('base', img);
    table.addImage('rollover', img2);

    table.onMouseOver = function() {
      this.changeAnimation("rollover");
    }

    table.onMouseOut = function() {
      this.changeAnimation("base");
    }

    table.onMousePressed = function() {
      var msg = this.position.x + ' ' + this.position.y;
      println(msg);
      socket.emit('message', msg);
    }

    clients[c] = {'mac': null, 'ip': null, 'loc': myLoc, 'status': null, 'sprite': table};
  }
}

function draw() {
  drawSprites();
}

function readData (data) {
  println(data);
}

// when new data comes in the websocket, read it:
socket.on('message', readData);
