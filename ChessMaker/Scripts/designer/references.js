var allLinkData = {};
function populateLinkData() {
    var data = $('#linkData').val().split(';');
    for (var i = 0; i < data.length; i++) {
        var parts = data[i].split(':');
        if (parts.length != 3)
            continue;

        var dir = parts[1];
        var from = parts[0];
        var to = $('#' + parts[2]);

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
    return doRenameCell(cell, newName) != null;
}

function doRenameCell(cell, newName) {
    // is this name already used? abort if so
    if ($('#render path.cell#' + newName).length > 0)
        return null;

    var oldName = cell.attr('id');
    cell.attr('id', newName);

    $('#render text')
        .filter(function () { return $(this).text() === oldName; })
        .text(newName);

    $('#renameData').val($('#renameData').val() + ';' + oldName + ':' + newName);

    // now rename the link data
    allLinkData[newName] = allLinkData[oldName];
    delete allLinkData[oldName];

    return newName;
}

function performAutoRename() {
    var initialCell = $('#render path.cell.selected');
    remClass(initialCell, 'selected');

    var dir1 = $('#bulkDir1').val();
    var dir2 = $('#bulkDir2').val();

    var start1 = $('#bulkDir1Start').val();
    var start2 = $('#bulkDir2Start').val();

    var prefix = $('#bulkDirPrefix').val();
    var middle = $('#bulkDirMiddle').val();
    var suffix = $('#bulkDirSuffix').val();

    renameRecursive(initialCell, dir1, dir2, 1, 1, prefix, middle, suffix);
    remClass($('#render path.cell.renamed'), 'renamed');
}

function renameRecursive(cell, dir1, dir2, nextRef1, nextRef2, prefix, middle, suffix) {
    var cellID = doRenameCell(cell, prefix + nextRef1 + middle + nextRef2 + suffix);
    addClass(cell, 'renamed');

    var dir1cell = allLinkData[cellID][dir1];
    if (typeof dir1cell != 'undefined' && !hasClass(dir1cell[0], 'renamed'))
        renameRecursive(dir1cell, dir1, dir2, nextRef1 + 1, nextRef2, prefix, middle, suffix);

    var dir2cell = allLinkData[cellID][dir2];
    if (typeof dir2cell != 'undefined' && !hasClass(dir2cell[0], 'renamed'))
        renameRecursive(dir2cell, dir1, dir2, nextRef1, nextRef2 + 1, prefix, middle, suffix);
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