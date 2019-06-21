$(document).ready(function() {
	var socket = io();

	var ledStatus = "OFF";
	var fanStatus = "OFF";
	$("#ledStatus").html(ledStatus);
	$("#fanStatus").html(fanStatus);
	socket.emit("web-join-room");

	socket.on("server-join-success", function(){
		socket.emit("web-req-update-led");
		socket.emit("web-req-update-fan");
	});

	socket.on("web-update-led",function(data){
		ledStatus=data;
		$("#ledStatus").html(ledStatus);
	});
	socket.on("web-update-fan",function(data){
		fanStatus=data;
		$("#fanStatus").html(fanStatus);
	});
	
		

	$("#btON").click(function() {
		if (ledStatus=="OFF")
		{
			ledStatus="ON";
			socket.emit("web-turn-on-off-led",ledStatus);
			$("#ledStatus").html(ledStatus);
		}
	});

	$("#btOFF").click(function() {
		if (ledStatus=="ON")
		{
			ledStatus="OFF";
			socket.emit("web-turn-on-off-led",ledStatus);
			$("#ledStatus").html(ledStatus);
		}
	});

	$("#btON_fan").click(function() {
		if (fanStatus=="OFF")
		{
			fanStatus="ON";
			socket.emit("web-turn-on-off-fan",fanStatus);
			$("#fanStatus").html(fanStatus);
		}
	});

	$("#btOFF_fan").click(function() {
		if (fanStatus=="ON")
		{
			fanStatus="OFF";
			socket.emit("web-turn-on-off-fan",fanStatus);
			$("#fanStatus").html(fanStatus);
		}
	});

	
});
