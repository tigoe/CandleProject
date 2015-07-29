var app = {
  // initialize runs when the app starts
  initialize: function() {
    this.bindEvents();
  },

// bindEvents adds listeners for the DOM elements:
  bindEvents: function() {
    var login = document.getElementById('login'),
      logout = document.getElementById('logout'),
      burst = document.getElementById('burst'),
      clientButton = document.getElementById('clientButton');

    login.addEventListener('click', this.login, false);
    logout.addEventListener('click', this.logout, false);
    burst.addEventListener('click', this.burst, false);
    clientButton.addEventListener('click', this.getClients, false);
  },

  login: function() {
    console.log(event.target.id);
    $.get('/login', function(data){
      app.showMessage(data);
    });
  },

  logout: function() {
    $.get('/logout', function(data){
      app.showMessage(data);
    });
  },

  getClients: function() {
    $.get('/clients', function(data){
      app.showClients(data);
    });
  },

  burst: function() {
    $.get('/burst', function(data){
      app.showMessage(data);
    });
  },

  showClients: function(clientList) {
    clients.innerHTML = 'Clients: <br>' + clientList;
  },

  showMessage: function(message) {
    var messageDiv = document.getElementById('messageDiv');
    console.log(message);
      messageDiv.innerHTML = message;
  }
}

app.initialize();
