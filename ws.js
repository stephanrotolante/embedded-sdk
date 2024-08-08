const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8765 });

wss.on('connection', function connection(ws) {
    console.log("connection");

    setInterval(() => {
        ws.send("hi");
    }, 2000);
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
        ws.send(message);
    });
});