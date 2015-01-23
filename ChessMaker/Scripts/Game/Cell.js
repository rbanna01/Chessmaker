"use strict";

function Cell(name, links) {
    this.coordX = 0;
    this.coordY = 0;
}

Cell.prototype.getImage = function () {
    return document.getElementById(this.name);
};