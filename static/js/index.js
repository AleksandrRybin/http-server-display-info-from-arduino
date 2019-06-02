$( document ).ready(() => {    
    var check_interval = setInterval(() => {
        $.get('/info', data => {
            $('#p-touches').text(data.touches);
            $('#p-vibrs').text(data.vibrs);
            $('#p-total').text(data.total);
        });
    }, 100);
})