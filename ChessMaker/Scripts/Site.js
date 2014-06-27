var navdrawer = document.querySelector('.navdrawer');
var appbar = document.querySelector('.app-bar');

var menuBtn = document.querySelector('.menu');
menuBtn.addEventListener('click', function () {
    var isOpen = navdrawer.classList.contains('open');
    if (isOpen) {
        appbar.classList.remove('open');
        navdrawer.classList.remove('open');
    } else {
        appbar.classList.add('open');
        navdrawer.classList.add('open');
    }
}, true);


$(function () {
    
});