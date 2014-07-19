$(function () {
    populateAbsDirs();

    $('#absDirList li').hover(absDirMouseOver, absDirMouseOut).click(absDirClick);

    //$('#render path.cell, #render line')
        //.click(elementClicked);
});

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
    var selected = $('#absDirList li.selected');
    if (selected.length > 0) {
        selected.removeClass('selected ui-state-error');
        if (selected[0] == this)
            return;

        $('#render .marker').hide();
        var dir = $(this).attr('dir');
        $('#render .marker[dir="' + dir + '"]').show();
    }
    
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

        from = from.pathSegList.getItem(0);
        to = to.pathSegList.getItem(0);

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
                .attr('dir', dir)
                .attr('marker-end', 'url(#arrowhead)')
                .appendTo($('#render'));

        output.children().sort(sortDirs).appendTo(output);
    }
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