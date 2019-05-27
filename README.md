# `bitalino` UNDER DEVELOPMENT

Only Linux is currently supported. There will be an experimental wrapper of the cpp-api, before native support of Windows and MacOS.

This package has only been tested on a linux system running Archlinux. Feel free to publish issues.

# Dependencies

Every module needs `node-gyp` installed as well as `bluez` and `libbluetooth-dev` on linux systems.

# Installation

```
npm install --save bitalino
```

# Modules

## BITalino

This is the native NodeJS module for use with the BITalino.

### How to use

```javascript
const BITalino = require('bitalino').BITalino;

BITalino.createBITalino('98:D3:31:30:26:43', null, function(bitalino) {
    bitalino.send(0xFE); // Simulation mode.
    // bitalino.start();
    while(1) {
        console.log(bitalino.receive(6, 10));
    }
});
```

### API

#### Factory
|Method|Parameters|Return|Description|
|---|---|---|---|:---:|
|`constructor(address, timeout = null, callback)`|`constructor(String, null, Function)`|`void`|Creates a BITalino object and executes the callback. The function passed as callback will receive a BITalino object instance as its parameter.|

#### BITalino

|Method|Return|Description|Required Version|
|---|---|---|:---:|
|`send(Number byte)`|`void`|Sends a 8-bit signal to the BITalino|*|
|`start(Number samplingRate, Array analogChannels)`|`void`|Signal the BITalino to start streaming data with a given sampling rate (1, 10, 100 or 1000) through the given number of analog channels. Defaults to sampling rate 1 on all channels|*|
|`stop()`|`void`|Stops the stream of data|*|
|`receive(Number numberOfChannels, Number numberOfFrames)`|`Array`|Receive the specified number of frames |*|
|`battery(Number value = 0)`|`void`|Set the battery threshold|*|
|`close()`|`void`|Closes the connection|*|
|`version()`|`void`|Get BITalino's version|*|
|`state()`|`Object`|Get a state frame from BITalino|>= 4.2|
|`pwm(Number output = 255)`|`void`||>= 4.2|

## BITalinoWrapped

A wrapper of the BITalino's C++ API.
**WARNING**: This module is only intended for use while the main module is under development.
Although it might have features that are not currently implemented on the main module, this module does not handle errors.

Check the BITalino's C++ API documentation [here](http://bitalino.com/docs/cpp-api/annotated.html).