const router = require('express').Router();
const fs = require('fs');

var data = {
    'touches' : 0,
    'vibrs' : 0,
    'total' : 0
};

if (fs.existsSync('data.json')) {
    data = JSON.parse(fs.readFileSync('data.json'));
}

router.post('/update', (req, res) => {
    data['touches'] = req.body.touches;
    data['vibrs'] = req.body.vibrs;
    data['total'] = req.body.total;

    fs.writeFile('data.json', JSON.stringify(data), err => {
        console.log(`error write file : ${err}`);
    });
});

router.get('/', (req, res) => {
    res.render('index.pug', data);
});

router.get('/info', (req, res) => {
    res.json(data);
})

module.exports = router;