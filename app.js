const express = require('express');
const path = require('path');

const router = require('./router');

var app = express();

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');
app.use(express.static(path.join(__dirname, 'static')));

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use('/', router);

app.listen(process.env.PORT || 5000);