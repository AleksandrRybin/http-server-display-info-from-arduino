const router = require('express').Router();
const fs = require('fs');

var data = {
    touches : 0,
    vibrs : 0,
    total : 0,
    first_connected : false
};

router.post('/update', (req, res) => {
    if (req.body.touches !== undefined &&
        req.body.vibrs !== undefined &&
        req.body.total !== undefined &&
        req.body.first_connected !== undefined) {
            
            data.touches = req.body.touches;
            data.vibrs = req.body.vibrs;
            data.total = req.body.total;
            data.first_connected = req.body.first_connected;

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

router.get('/', (req, res) => {
    res.render('index.pug', data);
});

router.get('/info', (req, res) => {
    res.json(data);
})

module.exports = router;