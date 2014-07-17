function addClass(elems, className) {
    elems.each(function () {
        var classes = this.getAttribute('class');
        var spaced = ' ' + classes + ' ';
        if (spaced.indexOf(className) != -1)
            return;
        this.setAttribute('class', classes + ' ' + className);
    });
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
    var svg = $('#render')[0];
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

$(function () {
    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });
});