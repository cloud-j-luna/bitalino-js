const ErrorCode = {
    INVALID_ADDRESS: 'The specified address is invalid.',
    INVALID_PLATFORM: 'This platform does not support bluetooth connection.',
    CONTACTING_DEVICE: 'The computer lost communication with the device.',
    DEVICE_NOT_IDLE: 'The device is not idle.',
    DEVICE_NOT_IN_ACQUISITION: 'The device is not in acquisition mode.',
    INVALID_PARAMETER: 'Invalid parameter.',
    INVALID_VERSION: 'Only available for Bitalino 2.0.',
    IMPORT_FAILED: 'Please connect using the Virtual COM Port or confirm that PyBluez is installed; bluetooth wrapper failed to import with error: ',
    CHANNEL_NOT_FOUND: 'No channel found, verify that your adapter is working properly.',
    NOT_SUPPORTED: 'Feature not supported yet.'
};

// Constants
const VCC = 3.3; // Volts

const BITalino = class BITalino {
    constructor(address, timeout = null, callback) {
        const macRegex = /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/;
        const checkMatch = macRegex.test(address);
        this.blocking = !timeout;

        if (!this.blocking) {
            throw new Error(ErrorCode.NOT_SUPPORTED);
        }

        if (!this.blocking) {
            try {
                this.timeout = Number(timeout);
            } catch (e) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
        }

        if (checkMatch) {
            this.wifi = false;
            if (process.platform === 'linux') { // Only supports linux.
                const bluetooth = require('./build/Release/bcomm');

                try {
                    bluetooth.connect(address);
                } catch(e) {
                    throw e;
                }

                this.socket = bluetooth;
            
                setTimeout(() => {
                    setupVersion();
                    callback(this);
                }, 100);
            } else {
                throw new Error(ErrorCode.INVALID_PLATFORM);
            }
        } else {
            throw new Error(ErrorCode.INVALID_ADDRESS);
        }

        this.started = false;
        this.address = address;

        const setupVersion = () => {
            const version = this.version();
            var versionNumber = Number(version.split(/[vV]{1}/)[1]);
            this.isBitalino2 = (versionNumber >= 4.2);
        }
    }

    start(samplingRate = 1000, analogChannels = [0, 1, 2, 3, 4, 5]) {  
        if (this.started === false) {
            if(![1, 10, 100, 1000].includes(Number(samplingRate))) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }

            const samplingRatesMap = {};
            samplingRatesMap['1'] = 0;
            samplingRatesMap['10'] = 1;
            samplingRatesMap['100'] = 2;
            samplingRatesMap['1000'] = 3;
            
            // CommandSRate: <Fs>  0  0  0  0  1  1
                            
            if(!(analogChannels instanceof Array)) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
            
            if(analogChannels.length === 0 || analogChannels.length > 6) { // missing verifications.
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }

            this.send((samplingRatesMap[samplingRate] << 6) | 0x03);
            
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

    state() {
        if(!this.isBitalino2) {
            throw new Error(ErrorCode.INVALID_VERSION);
        } else if(this.started) {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE);
        } else {
            this.send(0x0B);
        
            return this.socket.readState();
        }
    }

    stop() {
        if(this.started) {
            this.send(0x00);
        } else if(this.isBitalino2) {
            this.send(0xFF);
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IN_ACQUISITION);
        }

        this.started = false;
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
            return versionStr.slice(versionStr.indexOf('BITalino'), -1);
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE);
        }
    }

    pwm(output = 255) {
        if(this.isBitalino2) {
            if(0 <= output <= 255) {
                this.send(0xA3);
                this.send(output);
            } else {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
        } else {
            throw new Error(ErrorCode.INVALID_VERSION);
        }
    }

    receive(numberOfChannels, numberOfFrames) {
        const samples = [];
        if (this.serial) {
            throw new Error(ErrorCode.NOT_SUPPORTED);
        } else {
            if (!this.blocking) {
                throw new Error(ErrorCode.NOT_SUPPORTED);
            }

            while(numberOfFrames-- > 0) {
                samples.push(this.socket.readFrame(numberOfChannels));
            }

        }

        return samples;
    }

    battery(value = 0) {
        if (self.started == false) {
            value = Number(value);
            if (0 <= value && value <= 63) {
                // CommandBattery: <battery threshold> 0  0
                const commandBattery = value << 2;
                this.send(commandBattery);

            } else {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE);
        }
    }

    send(data) {
        if(this.wifi) {
            this.socket.write(Buffer.from([data], 'utf-8'), (err) => {
                if(err) {
                    throw new Error(err);
                }
            });
        } else {
            this.socket.send(data);
        }
    }

    close() {
        if(this.wifi) {
            try {
                this.socket.shutdown();
                this.socket.recv(1024);
                this.socket.close();
            } catch(e) {
                this.socket.close();
            }
        } else {
            this.socket.close();
        }
        
    }
};

module.exports.ErrorCode = ErrorCode;
module.exports.VCC = VCC;
module.exports.createBITalino = function (address, timeout = null, callback) {
    new BITalino(address, timeout, callback);
};
module.exports.BITalino = BITalino;