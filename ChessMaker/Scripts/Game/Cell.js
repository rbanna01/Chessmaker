"use strict";

function Cell(name, links) {
}

Cell.prototype.getImage = function () {
    return document.getElementById(this.name);
};