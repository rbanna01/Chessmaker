function PieceType() {
    this.name = null;
    this.value = 1;
    this.notation = '?';
    this.capturedAs = null;
    this.appearances = {};
    this.moves = [];
    //this.promotionOpportunities = [];
}

PieceType.allTypes = {};

// read definitions from xml
PieceType.parseAll = function (piecesNode, defs) {
    $(piecesNode).children().each(function () {
        var type = PieceType.parse(this, defs);

        if (PieceType.allTypes.hasOwnProperty(type.name)) // check for dupes
            throw "Duplicate piece types detected: two piece types exist with the name '" + type.name + "'. This is not allowed.";
        PieceType.allTypes[type.name] = type;
    });

    // resolve references to other types
    for (key in PieceType.allTypes) {
        var type = PieceType.allTypes[key];

        if (type.capturedAs != null)
            type.capturedAs = PieceType.allTypes[type.capturedAs];
        else
            type.capturedAs = type;
        /*
        for (var i = 0; i < type.promotionOpportunities.length; i++)
            type.promotionOpportunities[i].resolveOptions(definitions);
        */
    }
};

PieceType.parse = function (xmlNode, defs) {
    var type = new PieceType();

    type.name = xmlNode.getAttribute('name');

    var value = xmlNode.getAttribute('value');
    if (value !== undefined)
        type.value = value;

    var notation = xmlNode.getAttribute('notation');
    if (notation !== undefined)
        type.notation = notation;
    
    var capturedAs = $(xmlNode).children("capturedAs");
    if (capturedAs !== undefined)
        type.capturedAs = $(capturedAs).text();
    /*
    var movesNode = $(xmlNode).children("moves");
    if (movesNode !== undefined)
        $(movesNode).children().each(function () {
            type.allMoves.push(MoveDefinition.parse(this, true));
        });
    
    var specialNode = $(xmlNode).children("special");
    if (specialNode != undefined)
        $(specialNode).children().each(function () {
            if (this.tagName == "royal") // consider: while these properties should remain on pieces IN CODE (for game logic's sake) - shouldn't royalty in the DEFINITION be handled via victory conditions? lose when any/all pieces of given type are checkmated/captured/are in check/aren't in check? loading code could then apply royal / antiroyal values
                type.royalty = PieceType.RoyalState.Royal;
            else if (this.tagName == "anti_royal")
                type.royalty = PieceType.RoyalState.AntiRoyal;
            else if (this.tagName == "immobilize")
                type.immobilizations.push(Immobilization.parse(this));
            else // consider: other special types: blocks (as per immobilize, but instead prevents pieces entering a square), kills (kills pieces in target squares without expending a move)
                throw "Unexpected node name in piece's \"special\" tag: " + this.tagName;
        });
    */
    /*
    $(xmlNode).children("promotion").each(function () {
        type.promotionOpportunities.push(PromotionOpportunity.parse(this));
    });
    */

    $(xmlNode).children('appearance').each(function () {
        var playerName = this.getAttribute('player');
        var defID = type.name + '_' + playerName;

        var def = SVG('g');
        def.setAttribute('class', 'piece');
        def.setAttribute('id', defID);

        var trans = this.getAttribute('transform');
        if (trans !== undefined)
            def.setAttribute('transform', trans);

        def.innerHTML = this.innerHTML;
        defs.appendChild(def);

        type.appearances[playerName] = '#' + defID;
    });

    return type;
};