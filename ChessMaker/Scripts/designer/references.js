var allLinkData = {};
function populateLinkData() {
    var data = $('#linkData').val().split(';');
    for (var i = 0; i < data.length; i++) {
        var parts = data[i].split(':');
        if (parts.length != 3)
            continue;

        var dir = parts[1];
        var from = parts[0];
        var to = document.getElementById(parts[2]); // the actual object, to facilitate finding it easily after renaming

        if (from == null || to == null)
            continue;

        if (!allLinkData.hasOwnProperty(from))
            allLinkData[from] = {};

        allLinkData[from][dir] = to;
    }
}

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

    // now rename the link data
    allLinkData[newName] = allLinkData[oldName];
    delete allLinkData[oldName];

    return true;
}

function performAutoRename() {
    // wow, we don't actually have a way to navigate the JS tree. I guess we need markers!
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
        width: 540,
        height: 415,
        buttons: {
            "OK": function () {
                performAutoRename();
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

    $('#bulkDir1Type').change(function () {
        $('#bulkDir1Start').val($(this).val());
    });

    $('#bulkDir2Type').change(function () {
        $('#bulkDir2Start').val($(this).val());
    });

    populateLinkData();

    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });
});