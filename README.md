# `bitalino` UNDER DEVELOPMENT

Only linux is currently supported. There will be an experimental wrapper of the cpp-api, before native support of Windows and MacOS.

## Dependencies

Every module needs `node-gyp` installed as well as `bluez` and `libbluetooth-dev` on linux systems.

## Installation

```
npm install --save bitalino
```

## Modules

### BITalino

This is the native NodeJS module for use with the BITalino.

#### How to use

```javascript
const BITalino = require('bitalino').BITalino;

BITalino.createBITalino('98:D3:31:30:26:43', null, function(bitalino) {
    bitalino.send(0xFE); // Simulation mode.
    bitalino.start();
    while(1) {
        console.log(bitalino.receive(6, 100));
    }
});
```

### BITalinoRIOT

For use with the BITalino R-IoT ecosystem, which is composed of some of the available I/O's from the CC3200, this module is a replacement for the standard Bluetooth BITalino module, compatible only with the BITalino R-IoT.