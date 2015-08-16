var app = {
  // initialize runs when the app starts
  initialize: function() {
    this.bindEvents();
    this.getClients();    // initialize the list of UDP clients on the server
  },

  // bindEvents adds listeners for the DOM elements:
  bindEvents: function() {
    // get the buttons:
    var login = document.getElementById('login'),
    logout = document.getElementById('logout'),
    burst = document.getElementById('burst'),
    clientButton = document.getElementById('clientButton');

    // give the buttons listener functions:
    login.addEventListener('click', this.sendButtonCommand, false);
    logout.addEventListener('click', this.sendButtonCommand, false);
    burst.addEventListener('click', this.sendButtonCommand, false);
    clientButton.addEventListener('click', this.getClients, false);
  },
  // get the current client list from the server:
  getClients: function() {
    $.getJSON('/clients', function(data){
      app.showClients(data);
    });
  },

  // send the command from one of the other buttons:
  sendButtonCommand: function() {
    var address = event.target.name;      // get the name from the button's name
    var action = event.target.innerHTML;  // get the action from the button's inerHTML
    //concatenate a request string:
    var request = '/send/' + action + '/' + address;
    // make the request:
    $.get(request, function(data){
      app.showMessage(data);
    });
  },

  // when the server responds with the client list, this function
  // styles it as HTML:
  showClients: function(data) {
    clientDiv.innerHTML = "";     // clear the clientDiv
    var thisClientDiv;            // each client gets a sub-div
    // each client gets three action buttons:
    var buttons = ['login', 'logout', 'burst'];
    var thisButton;               // for iterating over the buttons
    for (thisClient in data) {
      // create the sub-div:
      thisClientDiv = document.createElement('div');
      // add the data for each client from the server:
      thisClientDiv.innerHTML = "IP address: " + data[thisClient].address;      thisClientDiv.innerHTML += '<br>'
      thisClientDiv.innerHTML += "MAC address: " + data[thisClient].macAddress;
      thisClientDiv.innerHTML += '<br>'
      thisClientDiv.innerHTML += "Last message time: " + data[thisClient].lastMessageTime;
      thisClientDiv.innerHTML += '<br>'

      // add three action buttons:
      for (var b in buttons) {
        thisButton = document.createElement('button');  // create the button
        thisButton.innerHTML = buttons[b];              // get the name from the list
        thisButton.name = data[thisClient].address;     // get the name from the client data
        // add an event listener for the button:
        thisButton.addEventListener('click', app.sendButtonCommand, false);
        thisClientDiv.appendChild(thisButton);          // add button to this client's div
      }

      // make alternating divs white or light blue:
      if (thisClient % 2 === 1) {
        thisClientDiv.style.background = '#ddddff';
      }
      clientDiv.appendChild(thisClientDiv);   // add this client's div to the main div
    }
  },

  // display any server messages:
  showMessage: function(message) {
    var messageDiv = document.getElementById('messageDiv');
    messageDiv.innerHTML = message;
  }
}

app.initialize();                   // start the app
setInterval(app.getClients, 2000);  // refresh the client list every two seconds
