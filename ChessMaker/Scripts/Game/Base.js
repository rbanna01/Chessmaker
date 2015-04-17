"use strict";

function SVG(tag) {
    return document.createElementNS('http://www.w3.org/2000/svg', tag);
}

function addClass(elems, className) {
    var classNameSpaced = ' ' + className + ' ';
    elems.each(function () {
        addClassSingle(this, className, classNameSpaced);
    });
}

function addClassSingle(elem, className, classNameSpaced) {
    if (classNameSpaced === undefined)
        classNameSpaced = ' ' + className + ' ';

    var classes = elem.getAttribute('class');
    var spaced = ' ' + classes + ' ';
    if (spaced.indexOf(classNameSpaced) != -1)
        return;
    elem.setAttribute('class', classes + ' ' + className);
}

function remClass(elems, className) {
    var spaced = ' ' + className + ' ';
    elems.each(function () {
        var classes = ' ' + this.getAttribute('class') + ' ';
        if (classes.indexOf(spaced) == -1)
            return;
        this.setAttribute('class', classes.replace(spaced, ' ').trim());
    });
}

function hasClass(elem, className) {
    var classes = ' ' + elem.getAttribute('class') + ' ';
    var spaced = ' ' + className + ' ';
    return classes.indexOf(spaced) != -1;
}

function calculateRatio() {
    var svg = document.getElementById('render');
    var viewBox = svg.getAttribute('viewBox').split(' ');
    var width = parseFloat(viewBox[2]);
    var height = parseFloat(viewBox[3]);
    svg.setAttribute('ratio', width / height);
}

function resizeBoard() {
    var svg = $('#render');
    var ratio = parseFloat(svg.attr('ratio'));

    var parent = svg.parent();
    var parentWidth = parent.outerWidth(false);
    var parentHeight = parent.outerHeight(false);

    var width; var height;
    if (parentHeight * ratio > parentWidth) {
        width = parentWidth;
        height = Math.floor(parentWidth / ratio);
    }
    else {
        width = Math.floor(parentHeight * ratio);
        height = parentHeight;
    }
    svg.css('width', width + "px").css('height', height + "px");
}