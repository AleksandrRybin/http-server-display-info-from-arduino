const express = require('express');
const path = require('path');

const router = require('./router');

var app = express();

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static(path.join(__dirname, 'static')));

app.use('/', router);

app.listen(5000 || process.env.PORT, '0.0.0.0');