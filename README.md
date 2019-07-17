# `bitalino` UNDER DEVELOPMENT

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/922a9f6befcb4ceb90eeef4d40665a0a)](https://app.codacy.com/app/joao-luna-98/bitalino-js?utm_source=github.com&utm_medium=referral&utm_content=joao-luna-98/bitalino-js&utm_campaign=Badge_Grade_Settings)

Only Linux is currently supported.

This package has only been tested on a linux system running Archlinux. Feel free to publish issues.

## Dependencies

Every module needs `node-gyp` installed as well as `bluez` and `libbluetooth-dev` on linux systems.

## Installation

```bash
npm install --save bitalino
```

## Modules

### BITalino

This is the native NodeJS module for use with the BITalino.

#### How to use

**Remember** the BITalino should be paired first.

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

For details on BITalino Micro Controller Unit visit [BITalino's official documentation](https://bitalino.com/datasheets/REVOLUTION_MCU_Block_Datasheet.pdf).

##### Factory
|Method|Return|Description|
|---|---|---|
|`createBITalino(String address, timeout = null, Function callback)`|`void`|Creates a BITalino object and executes the callback. The function passed as callback will receive a BITalino object instance as its parameter.|

##### BITalino

|Method|Return|Description|Required Version|
|---|---|---|:---:|
|`send(Number byte)`|`void`|Sends a 8-bit message to the BITalino|*|
|`start(Number samplingRate, Array analogChannels)`|`void`|Message the BITalino to start streaming data with a given sampling rate (1, 10, 100 or 1000) through the given number of analog channels. Defaults to sampling rate 1 on all channels|*|
|`stop()`|`void`|Stops the stream of data|*|
|`receive(Number numberOfChannels, Number numberOfFrames)`|`Array`|Receive the specified number of frames |*|
|`battery(Number value = 0)`|`void`|Set the battery threshold|*|
|`close()`|`void`|Closes the connection|*|
|`version()`|`void`|Get BITalino's version|*|
|`state()`|`Object`|Get a state frame from BITalino|>= 4.2|
|`pwm(Number output = 255)`|`void`||>= 4.2|

### BITalinoWrapped

BITalino wrapped was removed recently, check older branches if you still want to use it.