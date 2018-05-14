var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
var engineManualControllMovement;
var stateON;

connection.onopen = function () {
    connection.send('Connect ' + new Date());
};

connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
};

//handle message from server:
connection.onmessage = function (e) {
  var messageFromServer = e.data;
  
  //get state ON/OFF  
  checkStateOnOffFromServer(messageFromServer);
  //get status of manual/auto control
  checkManualOrAutomaticControll(messageFromServer);
  
  console.log('Message from server: ', messageFromServer);
};

connection.onclose = function () {
  console.log('WebSocket connection closed');
};

//check state ON/OFF from server and set HTML coresponding to received info
//state is received 
//a) on client connection
//b) on client click button "change status"
function checkStateOnOffFromServer(stateFromServer){
	if(stateFromServer.charAt(0) == 'S'){
		if(stateFromServer.charAt(1) == '1'){ //S1:active;S2:inactive
		    stateON = true;
			$( "#logState" ).css('background-color', 'green');
			$( "#logState" ).text( "Urządzenie aktywne" );
			$("#toggleOnOff span").text("Wyłącz");
		}else if(stateFromServer.charAt(1) == '0'){
			stateON = false;
			$( "#logState" ).css('background-color', 'red');
			$( "#logState" ).text( "Urządzenie nieaktywne" );
			$("#toggleOnOff span").text("Włacz");				
		}
	}	
	
}


function sendRGB () {
  var r = document.getElementById('r').value** 2 / 1023;
  var g = document.getElementById('g').value** 2 / 1023;
  var b = document.getElementById('b').value** 2 / 1023;

  var rgb = r << 20 | g << 10 | b;
  var rgbstr = '#' + rgb.toString(16);
  console.log('RGB: ' + rgbstr);
  connection.send(rgbstr);
}

function showCoords(event){
	if(engineManualControllMovement)
	{
		 var screenWidth = $("#cord").width();
		 var screenHeight = $("#cord").height();
		 var mousePositionX = ((event.clientX-8.5)-(screenWidth/2)); //8.5 to margines lewy
		 var mousePositionY = ((event.clientY-28.5)-(screenHeight/2)); //tak samo tylko ze gorny
		 var max = 181;
		 var valueX = (mousePositionX / screenWidth) * max;
		 var valueY = (mousePositionY / screenHeight) * max;
		
		 $( "#log" ).text( "pageX: " + parseInt(valueX) + ", pageY: " + parseInt(-valueY) );
		 sendData(parseInt(valueX), parseInt(-valueY));
	}
}

//get Manual or Auto servo-controlling
function checkManualOrAutomaticControll (stateFromServer) {
	if(stateFromServer.charAt(0) == 'M'){
		if(stateFromServer.charAt(1) == '1')
		{
			engineManualControllMovement = true;
			$( "#log" ).css('background-color', 'red');
			$( "#log" ).text( "Sterowanie manualne" );
				 //showCoords(event);
		}else{
			engineManualControllMovement = false;
			$( "#log" ).css('background-color', '#70dae0');
			$( "#log" ).text( "Sterowanie automatyczne" );
		}
	}
}

//button toggle ON/OFF state
function turnOffOn(){
	console.log("current ON/OFF state:" + stateON + " -> target state:" + (!stateON));
	connection.send("S"+Number(!stateON)); //send to server: S for state 1.on 0.off
}

//button manual/auto state
function turnkManualOrAutomaticControll(){
	console.log("current control state:" + engineManualControllMovement + " -> target state:" + (!engineManualControllMovement));
	connection.send("S"+Number(!engineManualControllMovement)); //send to server: S for state 1.on 0.off
}

//Sends mouse coordination data to ESP
function sendData (x,y) {
	if(engineManualControllMovement)
	{
	   var XCord = x;
	   var YCord = y;
	   var XYCord = 'CORD_' + XCord.toString() + "/"+YCord.toString();
	   console.log('Sending data X=' + XCord + " Y=" + YCord );
	   var obj = new Object();
	   obj.x = XCord;
	   obj.y = YCord;
	   var jsonString= JSON.stringify(obj);
	   connection.send(jsonString);
	}
}