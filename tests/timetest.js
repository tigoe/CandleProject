var start = new Date();

function checkTime() {
  var now = new Date();
  var diff = Date.parse(now)/1000 - Date.parse(start)/1000;
  console.log(diff);
  if (diff > 60) {
    console.log("minute");
  }

}
setInterval(checkTime, 4000);
