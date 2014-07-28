$(function () {
    populate();
});

function populate() {
    var relations = $('#relData').val().split(';');
    for (var i = 0; i < relations.length; i++) {
        var parts = relations[i].split(':');
        if (parts.length != 3)
            continue;

        var dir = parts[1];
        addRelativeDir(dir);
        populateLink(dir, parts[0], parts[2]);
    }
}

function addRelativeDir(dir) {
    // add a group box for it, with a row for every absolute dir, and a dropdown-list for where this relative dir leads to from eacht abs dir
}

function populateLink(dir, from, to) {
    // in the group box for "dir", find the "from" row and set the destination dropdown-list to the "to" value
}
