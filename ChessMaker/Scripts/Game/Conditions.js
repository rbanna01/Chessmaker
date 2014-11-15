function Conditions() {

}

Conditions.parse = function (nodes) {
    return new Conditions();
};

Conditions.prototype.isSatisfied = function (move, game) {
    return true; /* aye, well */
};