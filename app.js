const express = require('express');
const socketIO = require('socket.io');
const path = require('path');

var data = {
    touches : 0,
    vibrs : 0,
    total : 0
};

var app = express();
var io = socketIO(app);

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
    if (req.body.touches !== undefined &&
        req.body.vibrs !== undefined &&
        req.body.total !== undefined &&
        req.body.first_connected !== undefined) {
            
            data.touches = req.body.touches;
            data.vibrs = req.body.vibrs;
            data.total = req.body.total;

            if (req.body.first_connected) {
                io.emit('first_connection', data);
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
            error : 'some variables are undefined'
        });
    }
});

app.listen(process.env.PORT || 5000);