$( document ).ready(() => {    
    var socket = io();

    socket.on('update', (new_data) => {
        $('#p-touches').text(new_data.touches);
        $('#p-vibrs').text(new_data.vibrs);
        $('#p-total').text(new_data.total);
    });

    socket.on('first_connection', (data) => {
        $.notify("Arduino connected!", {
            globalPosition : 'center',
            class : 'success',
        });

        $('#p-touches').text(data.touches);
        $('#p-vibrs').text(data.vibrs);
        $('#p-total').text(data.total);
    });
})