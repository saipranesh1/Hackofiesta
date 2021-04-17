var lcd = new jsUpmI2cLcd.Jhd1313m1(6, 0x3E, 0x62);

var groveSensor = require('jsupm_grove');

var today = setInterval(function ()
    {
    var d = new Date();
    var b= d.toTimeString();
    lcd.setColor(0, 255, 0);
