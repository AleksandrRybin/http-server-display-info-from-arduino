$( document ).ready(() => {
    var socket = io();

    socket.on('update', (new_data) => {
        $.notify("New data arrived!", {
            className : 'info',
            autoHideDelay: 3000,
        });

        $('#p-touches').text(new_data.touches);
        $('#p-vibrs').text(new_data.vibrs);
        $('#p-total').text(new_data.total);
    });

    socket.on('first_connected', (data) => {
        $.notify("Arduino connected!", {
            className : 'success',
        });

        $('#p-touches').text(data.touches);
        $('#p-vibrs').text(data.vibrs);
        $('#p-total').text(data.total);
    });
})