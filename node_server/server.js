var sys = require('sys');
var http = require('http'),  
    io = require('./socket.io'), // for npm, otherwise use require('./path/to/socket.io') 
    //require( './lib/redis-client.js' )
  
server = http.createServer(function(request, response){ 
 // your normal server code 
 response.writeHead(200, {'Content-Type': 'text/html'}); 
 response.end('<h1>Hello world</h1>'); 
 response.end(); 
}); 
server.listen(8080);

/*
var client = require('redis').createClient();
client.info(function (err, info) {
    if (err) throw new Error(err);
    sys.puts("Redis Version is: " + info.redis_version);
    client.close();
});
*/

var socket = io.listen(server); 
//console.log('Client Disconnect');
console.log('Server Listening');

socket.on('connection', function(client) {
	//subscribe to client id channel when client connects
	console.log('Client Connected');
	/*
	r_client.subscribeTo("0_0", function(channel, message, pattern) {
    socket.broadcast(message);
});  
	 */
	
    client.on('message', function(message) {
      return socket.broadcast(message);
    });
    return client.on('disconnect', function() {
      return client.broadcast(JSON.stringify([['disconnect', client.sessionId]]));
      console.log('Client Disconnect');
      //unsubscribe from client channel when client disconnects
      //!!! implement!
    /* look up syntax
	r_client.unsubscribeTo("0_0", function(channel, message, pattern) {});  
	 */
    });
  });

//console.log('Not Crashed Yet');

//redis = require('./lib/redis-client.js');
var redis = require("redis");
var r = redis.createClient(6379, '127.0.0.1');

//console.log('Not Crashed Yet');

//this client connects to the Redis instance for direct to client communications
//var r_client = redis.createClient(6379, '127.0.0.1');
//subscribe to message stream for map/world

r.subscribe("world_0_out", function(channel, message, pattern) {
        console.log("message received!");
        socket.broadcast(message);
        //console.log("message received!");
});

/*
//global admin messages
r.subscribe("global_admin", function(channel, message, pattern) {
        socket.broadcast(message);
});
*/

console.log("started");
