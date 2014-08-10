function cellClicked() {
    addClass($(this), 'selected');
    if ($('#rbSingle').prop('checked')) {
        var txtName = $('#txtName').val($('#render path.cell.selected').attr('id'));
        $('#singleRename').dialog('open');
        txtName[0].select(); // select the text only after showing the dialog
    }
    else
        $('#bulkRename').dialog('open');
}

function renameSelectedCell() {
    var newName = $('#txtName').val();
    var cell = $('#render path.cell.selected');
    return doRenameCell(cell, newName);
}

function doRenameCell(cell, newName) {
    // is this name already used? abort if so
    if ($('#render path.cell#' + newName).length > 0)
        return false;

    var oldName = cell.attr('id');
    cell.attr('id', newName);

    $('#render text')
        .filter(function () { return $(this).text() === oldName; })
        .text(newName);

    $('#renameData').val($('#renameData').val() + ';' + oldName + ':' + newName);
    return true;
}

function setupAutoRename() {
    // should also just be able to name this cell on its own
}

$(function () {
    $('#singleRename').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                if ( renameSelectedCell() ) {
                    remClass($('#render path.cell'), 'selected');
                    $(this).dialog("close");
                }
            },
            Cancel: function () {
                remClass($('#render path.cell'), 'selected');
                $(this).dialog("close");
            }
        }
    });

    $('#bulkRename').dialog({
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

    $('.popup').keyup(function (e) {
        if (e.keyCode != 13)
            return;
        $(this).parent().find('button:nth-child(1)').click();
    });

    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });
});