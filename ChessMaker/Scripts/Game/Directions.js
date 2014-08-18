var RelativeDirections = {};

RelativeDirections.parse = function(dirsNode) {
    $(dirsNode).children('relative').each(function () {
        var dirNode = $(this);
        var name = dirNode.attr('name');
        var relDir = {};

        dirNode.children().each(function () {
            var link = $(this);
            relD[link.attr('from')] = link.attr('to');
        });

        RelativeDirections[name] = relDir;
    });
}


var DirectionGroups = {};

DirectionGroups.parse = function (dirsNode) {
    $(dirsNode).children('group').each(function () {
        var groupNode = $(this);
        var name = groupNode.attr('name');
        var group = [];

        groupNode.children().each(function () {
            var link = $(this);
            group.push(link.attr('dir'));
        });

        DirectionGroups[name] = groupNode;
    });
}


function ParseDirections(boardNode, dirsNode) {
    // do we actually need a list of absolute directions, at all? If not, then no need for the board node here

    DirectionGroups.parse(dirsNode);
    RelativeDirections.parse(dirsNode);
};

// whether its a global, relative or group name, this should output an array of resultant global directions
function ResolveDirection(name, prevDir) {
    if (DirectionGroups.hasOwnProperty(name))
        return DirectionGroups[name];

    if (RelativeDirections.hasOwnProperty(name)) {
        var relDir = RelativeDirections[name];
        if (relDir.hasOwnProperty(prevDir))
            return [relDir[prevDir]];
        else
            return [];
    }

    return name;
}