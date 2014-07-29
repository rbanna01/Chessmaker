function deleteDir() {
    var selected = $('#absDirList li.selected');
    if (selected.length == 0)
        return;

    var dir = selected.attr('dir');
    $('#render .marker[dir="' + dir + '"]').remove();
    selected.remove();

    $('#lnkRename, #lnkDelete, #lnkMerge').addClass('disabled');
}

function doRename() {
    var oldName = $('#absDirList li.selected').attr('dir');
    var newName = $('#txtRename').val();

    // if no change, "succeed" but do nothing
    if (oldName == newName)
        return true;

    // ensure the new name is valid
    if (newName.trim() == '' || newName.indexOf(':') != -1 || newName.indexOf(';') != -1)
        return false;

    // ensure that the new name is unique
    if ($('#absDirList li[dir="' + newName.replace("'", "''").replace('"', '""') + '"]:not(.selected)').length > 0)
        return false;

    // update text and dir attribute
    $('#absDirList li.selected')
        .attr('dir', newName)
        .text(newName);

    // update all markers with this dir attribute
    $('#render .marker[dir="' + oldName + '"]').attr('dir', newName);

    return true;
}

function populateMerge() {
    var ddl = $('#ddlMerge');
    ddl.children().remove();

    $('#absDirList li:not(.selected)').each(function () {
        var dir = $(this).attr('dir');
        ddl.append($('<option />').val(dir).text(dir));
    });
}

function doMerge() {
    var mergeFromItem = $('#absDirList li.selected');
    var mergeFrom = mergeFromItem.attr('dir');
    var mergeInto = $('#ddlMerge').val();
    var mergeIntoItem = $('#absDirList li[dir="' + mergeInto.replace("'", "''").replace('"', '""') + '"]');

    if (mergeIntoItem.length == 0)
        return false;

    $('#render .marker[dir="' + mergeFrom + '"]').attr('dir', mergeInto);

    var nextItem = mergeFromItem.next();
    if (nextItem.length == 0)
        mergeIntoItem.click();
    nextItem.click();
    mergeFromItem.remove();
    return true;
}

function newAbsDir() {
    var newName = $('#txtNewName').val();

    // ensure the new name is valid
    if (newName.trim() == '' || newName.indexOf(':') != -1 || newName.index(';') != -1)
        return false;

    // ensure that the new name is unique
    if ($('#absDirList li[dir="' + newName.replace("'", "''").replace('"', '""') + '"]:not(.selected)').length > 0)
        return false;

    // add new list item
    $('#absDirList').append($('<li />').attr('class', 'ui-state-default').attr('dir', newName).text(newName));

    // select it
    $('#absDirList li[dir="' + newName.replace("'", "''").replace('"', '""') + '"]')
        .hover(absDirMouseOver, absDirMouseOut).click(absDirClick)
        .click();
    return true;
}

function absDirMouseOver() {
    if ($('#absDirList li.selected').length > 0)
        return;

    var dir = $(this).attr('dir');
    $('#render .marker[dir="' + dir + '"]').show();
    $(this).removeClass("ui-state-default");
    $(this).addClass("ui-state-active");
}

function absDirMouseOut() {
    if ($('#absDirList li.selected').length == 0) {
        var dir = $(this).attr('dir');
        $('#render .marker[dir="' + dir + '"]').hide();
    }
    
    $(this).addClass("ui-state-default");
    $(this).removeClass("ui-state-active");
}

function absDirClick() {
    firstCell = null;
    if ($(this).hasClass('selected'))
        $('#lnkRename, #lnkDelete, #lnkMerge').addClass('disabled');
    else
        $('#lnkRename, #lnkDelete, #lnkMerge').removeClass('disabled');

    var selected = $('#absDirList li.selected');
    if (selected.length > 0) {
        selected.removeClass('selected ui-state-error');
        if (selected[0] == this)
            return;

        $('#render .marker').hide();
    }
    var dir = $(this).attr('dir');
    $('#render .marker[dir="' + dir + '"]').show();
    $(this).toggleClass("selected ui-state-error");
}

function populateAbsDirs() {
    var output = $('#absDirList');
    var data = $('#linkData').val().split(';');
    for (var i = 0; i < data.length; i++) {
        var parts = data[i].split(':');
        if (parts.length != 3)
            continue;

        var dir = parts[1];

        var existing = output.find('li[dir="' + dir + '"]');
        if (existing.length == 0) {
            output.append('<li class="ui-state-default" dir="' + dir + '">' + dir + '</li>');
        }

        var from = document.getElementById(parts[0]);
        var to = document.getElementById(parts[2]);

        if (from == null || to == null)
            continue;

        addMarker(from, to, dir);
    }

    $('#render .marker').hide();
    output.children().sort(sortDirs).appendTo(output);
}

function addMarker(fromCell, toCell, dir) {
    var from = fromCell.pathSegList.getItem(0);
    var to = toCell.pathSegList.getItem(0);

    // line ends should be a fixed distance from their cell centers
    var dx = to.x - from.x, dy = to.y - from.y;
    var length = Math.sqrt(dx * dx + dy * dy);
    dx = dx / length; dy = dy / length; // unit lengths

    var distShort = 10;

    var startX = from.x + (dx * distShort);
    var startY = from.y + (dy * distShort);
    var endX = to.x - (dx * distShort);
    var endY = to.y - (dy * distShort);

    // add hidden indicator element to the svg
    var elem = $(SVG('line'))
            .attr('x1', startX)
            .attr('x2', endX)
            .attr('y1', startY)
            .attr('y2', endY)
            .attr('class', 'marker')
            .attr('marker-end', 'url(#arrowhead)')
            .attr('dir', dir)
            .attr('from', fromCell.getAttribute('id'))
            .attr('to', toCell.getAttribute('id'))
            .appendTo($('#render'));
}

function sortDirs(a, b) {
    a = a.getAttribute('dir');
    b = b.getAttribute('dir');
    // temp dirs should be sorted numerically, so that temp10 doesn't comes before temp2
    var aTemp = a.length > 4 && a.substr(0, 4) == 'temp';
    var bTemp = b.length > 4 && b.substr(0, 4) == 'temp';

    if (aTemp) {
        if (bTemp) {
            // both temp
            var aNum = parseInt(a.substr(4));
            var bNum = parseInt(b.substr(4));
            if (isNaN(aNum) || isNaN(bNum))
                return a > b ? 1 : -1;

            return aNum > bNum ? 1 : -1;
        }
        else
            return 1;
    }
    else if (bTemp)
        return -1;

    return a > b ? 1 : -1;
}

var firstCell = null;
function cellClicked(e) {
    var dir = $('#absDirList li.selected');
    if (dir.length == 0)
        return;
    dir = dir.attr('dir');

    if (firstCell == null) {
        firstCell = this;
        addClass($(this), 'selected');
        return;
    }
    remClass($(firstCell), 'selected');

    if (firstCell == this) {
        firstCell = null;
        return;
    }

    // is there already a marker between these cells, for this direction? If so, remove it. Otherwise, add one.
    var existing = $('#render .marker[dir="' + dir + '"][from="' + firstCell.getAttribute('id') + '"][to="' + this.getAttribute('id') + '"]');
    if (existing.length > 0)
        existing.remove();
    else
        addMarker(firstCell, this, dir);
    firstCell = null;
}

$(function () {
    populateAbsDirs();

    $('#absDirList li').hover(absDirMouseOver, absDirMouseOut).click(absDirClick);

    $('#lnkDelete').click(deleteDir);

    $('#rename').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                if (doRename())
                    $(this).dialog("close");
            },
            Cancel: function () {
                $(this).dialog("close");
            }
        }
    });

    $('#merge').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                if (doMerge())
                    $(this).dialog("close");
            },
            Cancel: function () {
                $(this).dialog("close");
            }
        }
    });

    $('#new').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                if (newAbsDir())
                    $(this).dialog("close");
            },
            Cancel: function () {
                $(this).dialog("close");
            }
        }
    });

    $('.popup').keyup(function (e) {
        if (e.keyCode != 13)
            return;
        $(this).parent().find('button:nth-child(1)').click();
    });

    $('#lnkRename').click(function () {
        var input = $('#txtRename').val($('#absDirList li.selected').attr('dir'));
        $("#rename").dialog('open');
        input[0].select(); // select the text only after showing the dialog
    });

    $('#lnkMerge').click(function () {
        populateMerge();
        $("#merge").dialog('open');
    });

    $('#lnkNew').click(function () {
        $('#new').dialog('open');
    });

    $('#render path.cell').click(cellClicked);

    $('#linksForm').submit(function () {
        var data = '';
        $('#render .marker').each(function () {
            var m = $(this);
            data += ';' + m.attr('from') + ':' + m.attr('dir') + ':' + m.attr('to');
        });
        $('#linkData').val(data);
    });

    $(document).keydown(function (e) {
        if ($(".ui-dialog").is(":visible"))
            return; // do nothing if a popup is open

        switch (e.which) {
            case 82:
                if ($('#absDirList li.selected').length == 0)
                    return;
                $('#lnkRename').click();
                break;
            case 77:
                if ($('#absDirList li.selected').length == 0)
                    return;
                $('#lnkMerge').click();
                break;
            case 78:
                $('#lnkNew').click();
                break;
            case 46:
                if ($('#absDirList li.selected').length == 0)
                    return;
                $('#lnkDelete').click();
                break;
            case 13:
                var selected = $('#absDirList li.selected');
                if (selected.length == 0)
                    $('#absDirList li:first').click();
                else if (e.shiftKey)
                    selected.prev().click();
                else
                    selected.next().click();
                break;
            default:
                return;
        }
        e.preventDefault();
    });

    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });
});