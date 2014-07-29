function populate() {
    var relations = $('#relData').val().split(';');
    for (var i = 0; i < relations.length; i++) {
        var parts = relations[i].split(':');
        if (parts.length != 3)
            continue;

        var dir = parts[1];

        if ($('#main .groupbox[dir="' + dir.replace('"', '""') + '"]').length == 0)
            addRelativeDir(dir);
        populateLink(dir, parts[0], parts[2]);
    }
}

var absDirTable = null;
var absDirOptions = null;
function addRelativeDir(dir) {
    // add a group box for it, with a row for every absolute dir, and a dropdown-list for where this relative dir leads to from eacht abs dir

    if (absDirTable == null) {
        absDirTable = '<table><tr><th>from</th><th>to</th></tr>';
        
        absDirOptions = '<option value="">(not applicable)</option>';
        $('#indicator .marker').each(function () {
            var absdir = $(this).attr('dir');
            absDirOptions += '<option value="' + absdir + '">' + absdir + '</option>';
            absDirTable += '<tr dir="' + absdir + '"><td>' + absdir + '</td><td><select class="toAbsDirs"></select></td></tr>';
        });
        absDirTable += '</table>';
    }

    var groupbox = $('<div/>')
        .attr('class', 'groupbox ui-widget ui-corner-all')
        .attr('dir', dir)
        .html('<h4>' + dir + '</h4>' + absDirTable);

    groupbox.find('select.toAbsDirs').html(absDirOptions);
    groupbox.appendTo($('#main'));
}

function populateLink(dir, from, to) {
    // in the group box for "dir", find the "from" row and set the destination dropdown-list to the "to" value
    $('#main .groupbox[dir="' + dir.replace('"', '""') + '"] tr[dir="' + from.replace('"', '""') + '"] select.toAbsDirs').val(to);
}

$(function () {
    populate();

    $('#addDir').button().click(function () {
        console.log("add new dir");
        addRelativeDir('some new dir');
    });
    $('#remDir').button().click(function () {
        console.log("remove selected dir");
    }).button('option', 'disabled', true);
});