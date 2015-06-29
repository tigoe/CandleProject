var socket = io();		      // socket.io instance. Connects back to the server


var clients = [];
var clientCount = 24;
var tableSize = 30;
var img, img2;
var chosenSprite = null;
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

    clients[c] = {'mac': null, 'ip': null, 'loc': myLoc, 'status': null, 'sprite': null};

      clients[c].sprite = createSprite(row, col);

    clients[c].sprite.addImage('base', img);
    clients[c].sprite.addImage('rollover', img2);

    clients[c].sprite.onMouseOver = function() {
      this.changeImage("rollover");
      if (chosenSprite === this) {
        this.position.x = mouseX;
        this.position.y = mouseY;
      }
    }

    clients[c].sprite.onMouseOut = function() {
      this.changeImage("base");
    }

    clients[c].sprite.onMouseReleased = function() {
      if (chosenSprite === this) {
        this.position.x = mouseX;
        this.position.y = mouseY;
      }

      this.mouseIsActive = false;
      chosenSprite = null;
      println("released");
    }

    clients[c].sprite.onMousePressed = function() {
      this.mouseIsActive = true;
      chosenSprite = this;
      var msg = this.position.x + ' ' + this.position.y;
      println(msg);
      socket.emit('message', msg);
    }
  }
}

function draw() {
  background(255);
  drawSprites();
}

function readData (data) {
  println(data);
}

// when new data comes in the websocket, read it:
socket.on('message', readData);
