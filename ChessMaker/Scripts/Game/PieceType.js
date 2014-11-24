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
    var typeNodes = piecesNode.childNodes;
    for (var i = 0; i < typeNodes.length; i++) {
        var typeNode = typeNodes[i];
        var type = PieceType.parse(typeNode, defs);

        if (PieceType.allTypes.hasOwnProperty(type.name)) // check for dupes
            throw "Duplicate piece types detected: two piece types exist with the name '" + type.name + "'. This is not allowed.";
        PieceType.allTypes[type.name] = type;
    }
    
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
    if (value != null) {
        value = parseInt(value);
        if (!isNaN(value))
            type.value = value;
    }

    var notation = xmlNode.getAttribute('notation');
    if (notation != null)
        type.notation = notation;

    var children = xmlNode.childNodes;
    for (var i = 0; i < children.length; i++) {
        var childNode = children[i];

        switch (childNode.tagName) {
            case 'capturedAs':
                type.capturedAs = childNode.textContent;
                break;
            case 'moves':
                var moves = childNode.childNodes;
                for (var j=0; j<moves.length; j++)
                    type.moves.push(MoveDefinition.parse(moves[j], true));
                break;
            /*
            case 'special':
                var specials = childNode.childNodes;
                for (var j=0; j<specials.length; j++)
                    var special = specials[j];

                    if (special.tagName == "royal") // consider: while these properties should remain on pieces IN CODE (for game logic's sake) - shouldn't royalty in the DEFINITION be handled via victory conditions? lose when any/all pieces of given type are checkmated/captured/are in check/aren't in check? loading code could then apply royal / antiroyal values
                        type.royalty = PieceType.RoyalState.Royal;
                    else if (special.tagName == "anti_royal")
                        type.royalty = PieceType.RoyalState.AntiRoyal;
                    else if (special.tagName == "immobilize")
                        type.immobilizations.push(Immobilization.parse(this));
                    else // consider: other special types: blocks (as per immobilize, but instead prevents pieces entering a square), kills (kills pieces in target squares without expending a move)
                        throw "Unexpected node name in piece's \"special\" tag: " + this.tagName;;
                break;    
            case 'promotion':
                var promos = childNode.childNodes;
                for (var j=0; j<promos.length; j++)
                    type.promotionOpportunities.push(PromotionOpportunity.parse(promos[j]);
                break;
            */

            case 'appearance':
                var playerName = childNode.getAttribute('player');
                var defID = type.name + '_' + playerName;

                var def = SVG('g');
                def.setAttribute('class', 'piece');
                def.setAttribute('id', defID);

                var trans = childNode.getAttribute('transform');
                if (trans !== undefined)
                    def.setAttribute('transform', trans);

                def.innerHTML = childNode.innerHTML;
                defs.appendChild(def);

                type.appearances[playerName] = '#' + defID;
                break;
        }
    }

    return type;
};