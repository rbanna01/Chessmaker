function Cell(name, links) {
    this.name = name;
    this.coordX = 0;
    this.coordY = 0;
    this.piece = null;
    this.links = {};

    var cell = this;
    links.each(function () {
        var dir = this.getAttribute('dir');
        var ref = this.getAttribute('to');

        cell.links[dir] = ref;
    });
}

Cell.prototype.loadSVG = function (xml) {
    var cell = SVG('path');
    cell.setAttribute('id', this.name);
    var path = xml.getAttribute('path');
    cell.setAttribute('d', path);

    var cssClass = 'cell ' + xml.getAttribute('fill');
    var border = xml.getAttribute('border');
    if (border != null)
        cssClass += ' ' + border + 'Stroke';
    cell.setAttribute('class', cssClass);

    // save off each cell's position
    var seg = cell.pathSegList.getItem(0);
    this.coordX = seg.x;
    this.coordY = seg.y;
    return cell;
};


Cell.prototype.getImage = function () {
    return document.getElementById(this.name);
};