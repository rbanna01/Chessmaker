function cellClicked() {
    addClass($(this), 'selected');
    $('#setup').dialog('open');
}

function setupAutoRename() {
    // should also just be able to name this cell on its own
}

$(function () {
    $('#setup').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                setupAutoRename();
                remClass($('#render path.cell'), 'selected');
                $(this).dialog("close");
            },
            Cancel: function () {
                remClass($('#render path.cell'), 'selected');
                $(this).dialog("close");
            }
        }
    });

    $('#render path.cell').click(cellClicked);

    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });
});