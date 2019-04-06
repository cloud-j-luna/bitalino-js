const addon = require('../build/Release/bluetooth');

addon.connect('98:D3:31:30:26:43');
addon.send(0x07);
console.log(addon.recv(24));
