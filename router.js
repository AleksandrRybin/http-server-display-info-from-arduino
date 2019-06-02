const router = require('express').Router();
const fs = require('fs');

var data = {
    'touches' : 0,
    'vibrs' : 0,
    'total' : 0
};

router.post('/update', (req, res) => {
    if (req.body.touches != null &&
        req.body.vibrs != null &&
        req.body.total != null) {
            data['touches'] = req.body.touches;
            data['vibrs'] = req.body.vibrs;
            data['total'] = req.body.total;

            res.json({
                'status' : 'okay',
                'time' : new Date(),
            });
    }
    else {
        res.json({
            'status' : 'failed',
            'error' : 'some variables are undefined'
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