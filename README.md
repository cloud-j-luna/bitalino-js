# `bitalino` UNDER DEVELOPMENT

Only Linux is currently supported.

This package has only been tested on a linux system running Archlinux. Feel free to publish issues.

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
    // bitalino.start();
    while(1) {
        console.log(bitalino.receive(6, 10));
    }
});
```

#### API

##### Factory
|Method|Parameters|Return|Description|
|---|---|---|---|
|`constructor(address, timeout = null, callback)`|`constructor(String, null, Function)`|`void`|Creates a BITalino object and executes the callback. The function passed as callback will receive a BITalino object instance as its parameter.|

##### BITalino

|Method|Parameters|Return|Description|Required Version|
|---|---|---|---|:---:|
|`send(data)`|`send(Number)`|`void`|Sends a 8-bit signal to the BITalino|*|
|`start(samplingRate, analogChannels)`|`start(Number, Array)`|`void`|Signal the BITalino to start streaming data with a given sampling rate (1, 10, 100 or 1000) through the given number of analog channels. Defaults to sampling rate 1 on all channels|*|
|`stop()`|`stop()`|`void`|Stops the stream of data|*|
|`receive(numberOfChannels, numberOfFrames)`|`receive(Number, Number)`|`Array`|Receive the specified number of frames |*|
|`battery(value = 0)`|`battery(Number)`|`void`|Set the battery threshold|*|
|`close()`|`close()`|`void`|Closes the connection|*|
|`version()`|`version()`|`void`|Get BITalino's version|*|
|`state()`|`state()`|`Object`|Get a state frame from BITalino|>= 4.2|
|`pwm(output = 255)`|`pwm(Number)`|`void`||>= 4.2|

### BITalinoWrapped

BITalino wrapped was removed recently, check older branches if you still want to use it.