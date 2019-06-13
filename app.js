const express = require('express');
const http = require('http');
const socketIO = require('socket.io');
const path = require('path');

var data = {
    touches : 0,
    vibrs : 0,
    total : 0
};

var app = express();
var server = http.createServer(app);
var io = socketIO(server);

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');
app.use(express.static(path.join(__dirname, 'static')));

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.get('/', (req, res) => {
    res.render('index.pug', data);
});

app.get('/info', (req, res) => {
    res.json(data);
});

app.post('/update', (req, res) => {
    let touches = JSON.parse(req.body.touches);
    let vibrs = JSON.parse(req.body.vibrs);
    let total = JSON.parse(req.body.total);
    let first_connected = JSON.parse(req.body.first_connected);

    if (Number.isInteger(touches)         &&
        Number.isInteger(vibrs)           &&
        Number.isInteger(total)           &&
        typeof(first_connected) == 'boolean') {
            
            if (first_connected) {
                io.emit('first_connected', data);
            }
            else {
                io.emit('update', data);
            }

            res.json({
                status : 'okay',
                time : new Date(),
            });
    }
    else {
        res.json({
            status : 'failed',
            error : 'some variables are undefined or incorrect'
        });
    }
});

server.listen(process.env.PORT || 5000);