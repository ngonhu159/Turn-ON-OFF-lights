//setup
var express = require('express');
var app = express();
app.use(express.static("./public"));
app.set("view engine","ejs");
app.set("views", "./views");
var port = 3000;
var server = require("http").Server(app);

//post-get
var bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({extended: false}));

//socket
var io = require("socket.io")(server);

//tao ket noi
io.on("connection", function(socket) {
	console.log(socket.id + " connected");
	socket.on("disconnect", function() {
		console.log(socket.id + " disconnected");
	});


	socket.on("esp-join-room", function(){
		socket.join("esp-client");
	});
	socket.on("web-join-room", function(){
		socket.join("web-client");
		socket.emit("server-join-success");
	});

	socket.on("web-req-update-led", function(){
		io.sockets.in("esp-client").emit("Req-update-led-from-web");
	});
	socket.on("web-req-update-fan", function(){
		io.sockets.in("esp-client").emit("Req-update-fan-from-web");
	});
	socket.on("Esp-update-led-to-web", function(ledStatus){
		io.sockets.in("web-client").emit("web-update-led",ledStatus);
	});
	socket.on("Esp-update-fan-to-web", function(fanStatus){
		io.sockets.in("web-client").emit("web-update-fan",fanStatus);
	});

	socket.on("web-turn-on-off-led", function(ledStatus) {
		io.sockets.in("esp-client").emit("Turn-on-off-led-form-web",ledStatus);
	});
	socket.on("web-turn-on-off-fan", function(fanStatus) {
		io.sockets.in("esp-client").emit("Turn-on-off-fan-form-web",fanStatus);
	});
	
});

app.get('/control', function(req, res, next) {
	res.render("control");
});


server.listen(process.env.PORT || port);
