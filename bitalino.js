const ErrorCode = {
    INVALID_ADDRESS: "The specified address is invalid.",
    INVALID_PLATFORM: "This platform does not support bluetooth connection.",
    CONTACTING_DEVICE: "The computer lost communication with the device.",
    DEVICE_NOT_IDLE: "The device is not idle.",
    DEVICE_NOT_IN_ACQUISITION: "The device is not in acquisition mode.",
    INVALID_PARAMETER: "Invalid parameter.",
    INVALID_VERSION: "Only available for Bitalino 2.0.",
    IMPORT_FAILED: "Please connect using the Virtual COM Port or confirm that PyBluez is installed; bluetooth wrapper failed to import with error: ",
    CHANNEL_NOT_FOUND: "No channel found, verify that your adapter is working properly.",
    NOT_SUPPORTED: "Feature not supported yet."
};

const BITalino = class BITalino {
    constructor(address, timeout = null, callback) {
        const macRegex = /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/
        const checkMatch = macRegex.test(address);
        this.blocking = !timeout;

        if (!this.blocking) {
            try {
                this.timeout = Number(timeout);
            } catch (e) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
        }
        if (checkMatch) {
            this.wifi = false;
            this.serial = false;
            if (process.platform === 'win32' || process.platform === 'linux') { // Only supports linux.
                const bluetooth = require('./build/Release/bcomm');

                bluetooth.connect('98:D3:31:30:26:43');

                console.log('connected');

                this.socket = bluetooth;
            
                setTimeout(() => {
                    setupVersion();
                    callback(this);
                }, 100);
            } else {
                throw new Error(ErrorCode.INVALID_PLATFORM);
            }
        } else if (address.substr(0, 3) === 'COM' && process.platform === 'win32'
            || address.substr(0, 5) === '/dev/' && process.platform !== 'win32') {   // BLE
            const btSerial = new (require('bluetooth-serial-port')).BluetoothSerialPort();
            btSerial.connect(address, 115200, function () {

                this.socket = btSerial;
            }, function () {
            });

            this.wifi = false;
            this.serial = true;
        } else if (address.replace(/[^:]/g, "").length) {
            const net = require('net');
            const dest = address.split(':');
            net.createConnection(dest[1], dest[0], function () {
                console.log('connected');
            });

            this.wifi = true;
            this.serial = false;
        } else {
            throw new Error(ErrorCode.INVALID_ADDRESS);
        }

        this.started = false;
        this.address = address;

        const setupVersion = () => {
            const split_string = 'v';
            const split_string_old = 'V';
            const version = this.version();
            if (version.indexOf(this.split_string) >= 0) {
                var versionNumber = Number(version.split(split_string)[1]);
            } else {
                var versionNumber = Number(version.slice(10, 13));
            }
            this.isBitalino2 = (versionNumber >= 4.2);
        }
    }

    start(samplingRate = 1, analogChannels = [0, 1, 2, 3, 4, 5]) {  
        if (this.started === false) {
            if(![1, 10, 100, 1000].includes(Number(samplingRate))) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }

            let commandSRate;
            
            // CommandSRate: <Fs>  0  0  0  0  1  1
            if(Number(samplingRate) === 1000) commandSRate = 3;
            else if(Number(samplingRate) === 100) commandSRate = 2;
            else if(Number(samplingRate) === 10) commandSRate = 1;
            else if(Number(samplingRate) === 1) commandSRate = 0;
                            
            if(!(analogChannels instanceof Array)) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
            
            if(analogChannels.length === 0 || analogChannels.length > 6) { // missing verifications.
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }

            this.send((commandSRate << 6) | 0x03);
            
            // CommandStart: A6 A5 A4 A3 A2 A1 0 1
            let commandStart = 0x01;
            for(const i of analogChannels) {
                commandStart = commandStart | 1 << (2+i);
            }
            
            this.send(commandStart);
            this.started = true;
            this.analogChannels = analogChannels;
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE);
        }
    }

    version() {
        if (this.started === false) {
            // CommandVersion: 00000111, 0x07, 7
            this.send(0x07);

            let char, versionStr = '';
            while(char !== '\n') {
                char = Buffer.from(this.socket.recv(1)).toString('utf-8');
                versionStr += char;
            }
            console.log(versionStr);
            return versionStr.slice(versionStr.indexOf('BITalino'), -1);
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE);
        }
    }

    receive(numberOfChannels, numberOfFrames) {
        if (this.serial) {
            throw new Error(ErrorCode.NOT_SUPPORTED);
        } else {
            if (!this.blocking) {
                throw new Error(ErrorCode.NOT_SUPPORTED);
                //ready = select.select([self.socket], [], [], self.timeout)
                //if (ready[0]) {
                    //pass
                //} else {
                //    throw new Error(ErrorCode.CONTACTING_DEVICE);
                //}
            }

            return this.socket.readFrame(numberOfChannels, numberOfFrames);
        }
    }

    battery(value = 0) {
        if (self.started == false) {
            if (0 <= Number(value) && Number(value) <= 63) {
                // CommandBattery: <battery threshold> 0  0
                const commandBattery = int(value) << 2;
                this.send(commandBattery);

            } else {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE)
        }
    }

    send(data) {
        if(this.serial || this.wifi) {
            this.socket.write(Buffer.from([data], 'utf-8'), (err) => {
                if(err) throw new Error(err);
            });
        } else {
            this.socket.send(data);
        }
    }

    close() {
        this.socket.close();
    }
}

exports.ErrorCode = ErrorCode;
exports.createBITalino = function (address, timeout = null, callback) {
    new BITalino(address, timeout, callback);
};