$( document ).ready(() => {
    $('#arduino-connected_display-close-btn').click(() => {
        $('#arduino-connected_display').css('display', 'none');
    })
    
    var check_interval = setInterval(() => {
        $.get('/info', data => {
            $('#p-touches').text(data.touches);
            $('#p-vibrs').text(data.vibrs);
            $('#p-total').text(data.total);
    
            if (data.first_connected) {
                $('#arduino-connected_display').css('display', 'block');
            }
        });
    }, 100);
})