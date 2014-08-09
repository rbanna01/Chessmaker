function populateDirList() {
    var output = $('#dirList');

    var data = $('#allDirs').val().split(';');
    for (var i = 0; i < data.length; i++) {
        var dir = data[i];
        output.append('<li class="ui-state-default" dir="' + dir + '">' + dir + '</li>');
    }
}

function dirMouseOver() {
    $(this).removeClass("ui-state-default");
    $(this).addClass("ui-state-active");
}

function dirMouseOut() {
    $(this).addClass("ui-state-default");
    $(this).removeClass("ui-state-active");
}

function dirClick() {
    $(this).toggleClass("selected ui-state-error");
    checkAddRemove();
}

function populateGroups() {
    var groups = $('#groupData').val().split(';');
    for (var i = 0; i < groups.length; i++) {
        if (groups[i].length == 0)
            continue;
        var group = groups[i].split(':');
        var groupList = addDirGroup(group[0]).find('ul');
        for (var j = 1; j < group.length; j++)
            addDirToGroup(groupList, group[j]);
    }
}

function addDirGroup(group) {
    $('.groupbox').removeClass('selected');

    var groupbox = $('<div/>')
        .attr('class', 'groupbox ui-widget ui-corner-all selected')
        .attr('group', group)
        .html('<h4>' + group + '</h4><ul class="dirs"></ul>')
        .click(groupBoxClicked);

    return groupbox.appendTo($('#main'));
}

function groupBoxClicked() {
    var box = $(this);
    var noneSelected;

    if (box.hasClass('selected')) {
        box.removeClass('selected');
        noneSelected = true;
    }
    else {
        $('.groupbox').removeClass('selected');
        box.addClass('selected');
        noneSelected = false;
    }

    $('#renameGroup, #deleteGroup').button('option', 'disabled', noneSelected);
    checkAddRemove();
}

function checkAddRemove() {
    var selectedDirs = $('#dirList li.selected');
    var selectedGroup = $('#main .groupbox.selected');

    if (selectedDirs.length > 0 && selectedGroup.length > 0)
        $('#lnkAddToGroup, #lnkRemoveFromGroup').removeClass('disabled');
    else
        $('#lnkAddToGroup, #lnkRemoveFromGroup').addClass('disabled');
}

function sortDirs(a, b) {
    a = a.getAttribute('dir');
    b = b.getAttribute('dir');
    return a > b ? 1 : -1;
}

function addDirToGroup(groupList, dir) {
    if (groupList.find('li[dir="' + dir.replace('"', '""') + '"]').length == 0)
        groupList.append('<li dir="' + dir + '">' + dir + '</li>');
}

$(function () {
    populateDirList();
    $('#dirList li').hover(dirMouseOver, dirMouseOut).click(dirClick);

    $('#new').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                var group = $('#txtNew').val();
                if ($('#main .groupbox[group="' + group.replace('"', '""') + '"]').length != 0)
                    return; // one already exists with this name
                addDirGroup(group);
                $('#txtNew').val('');
                $(this).dialog("close");
            },
            Cancel: function () {
                $(this).dialog("close");
            }
        }
    });

    $('#rename').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "OK": function () {
                var group = $('#txtRename').val();

                var selected = $('#main .groupbox.selected');
                if (selected.attr('group') != group){ // only make changes if the name has changed

                    if ($('#main .groupbox[group="' + group.replace('"', '""') + '"]').length != 0)
                        return; // one already exists with this name

                    selected.attr('group', group).find('h4').html(group);
                }

                $('#txtRename').val('');
                $(this).dialog("close");
            },
            Cancel: function () {
                $(this).dialog("close");
            }
        }
    });
    
    $('#addGroup').button().click(function () {
        $('#new').dialog('open');
    });

    $('#renameGroup').button().click(function () {
        $('#txtRename').val($('#main .groupbox.selected').attr('group'));
        $('#rename').dialog('open');
    }).button('option', 'disabled', true);

    $('#deleteGroup').button().click(function () {
        $('#main .groupbox.selected').remove();
        $('#renameGroup, #deleteGroup').button('option', 'disabled', true);
    }).button('option', 'disabled', true);

    $('#lnkAddToGroup').click(function () {
        var groupList = $('#main .groupbox.selected ul');
        $('#dirList li.selected').each(function () {
            addDirToGroup(groupList, $(this).attr('dir'));
        });

        groupList.children().sort(sortDirs).appendTo(groupList);
        return false;
    });

    $('#lnkRemoveFromGroup').click(function () {
        var groupList = $('#main .groupbox.selected ul');
        $('#dirList li.selected').each(function () {
            var dir = $(this).attr('dir');
            groupList.find('li[dir="' + dir.replace('"', '""') + '"]').remove();
        });
        return false;
    });

    $('.popup').keyup(function (e) {
        if (e.keyCode != 13)
            return;
        $(this).parent().find('button:nth-child(1)').click();
    });

    $('#groupForm').submit(function () {
        var data = '';
        $('#main .groupbox').each(function () {
            var group = $(this).attr('group');

            data += ';' + group;
            $(this).find('ul li').each(function () {
                var dir = $(this).attr('dir');
                data += ':' + dir;
            });
        });
        $('#groupData').val(data);
    });

    populateGroups();
});