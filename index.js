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
    constructor(macAddress, timeout = null, callback) {
        const macRegex = /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/
        const checkMatch = macRegex.test(macAddress);
        this.blocking = !timeout;

        if (!this.blocking) {
            try {
                this.timeout = Number(timeout);
            } catch (e) {
                throw new Error(ErrorCode.INVALID_PARAMETER);
            }
        }
        if (checkMatch) {   // Bluetooth
            if (process.platform === 'win32' || process.platform === 'linux') { // Only supports linux.
                const bluetooth = require('node-bluetooth');

                const device = new bluetooth.DeviceINQ();
                device.findSerialPortChannel(macAddress, (channel) => {

                    if (channel < 0) {
                        throw new Error(ErrorCode.CHANNEL_NOT_FOUND);
                    }

                    bluetooth.connect(macAddress, channel, (err, connection) => {
                        this.socket = connection;
                        if (err) return console.error(err);

                        connection.on('data', (buffer) => {
                            this.buffer = Buffer.concat([this.buffer, buffer]);
                        });

                        setupVersion();
                        callback(this);
                    });

                });

                this.wifi = false;
                this.serial = false;
            } else {
                throw new Error(ErrorCode.INVALID_PLATFORM);
            }
        } else if (x) {   // BLE
            try {
                const bluetooh = require('bluetooth');
            } catch (e) {
                throw new Error(ErrorCode.IMPORT_FAILED + str(e));
            }
        } else {
            throw new Error(ErrorCode.INVALID_ADDRESS);
        }

        this.started = false;
        this.macAddress = macAddress;

        const setupVersion = () => {
            this.split_string = '_v';
            this.split_string_old = 'V';
            const version = this.version();

            if (version.indexOf(this.split_string) >= 0) {
                var versionNumber = Number(version.split(split_string)[1]);
            } else {
                var versionNumber = Number(version.split(split_string_old)[1]);
            }
            this.isBitalino2 = (versionNumber >= 4.2);
        }
    }

    version() {
        if (this.started == False) {
            // CommandVersion: 00000111, 0x07, 7
            this.send(7);
            versionStr = '';
            while(true) {
                versionStr += this.receive(1).decode('utf-8')
                if (versionStr[-1] === '\n' && versionStr.indexOf('BITalino') >= 0) {
                    break;
                }
            }
            return versionStr[versionStr.indexOf('BITalino')] // REVIEW: Python version_str[version_str.index("BITalino"):-1]
        } else {
            throw new Error(ErrorCode.DEVICE_NOT_IDLE);
        }
    }

    receive(nBytes) {
        const data = Buffer.alloc(nBytes);
        const pointer = 0;
        if (this.serial) {
            throw new Error(ErrorCode.NOT_SUPPORTED);
        } else {
            while (pointer < nBytes) {
                if (!this.blocking) {
                    throw new Error(ErrorCode.NOT_SUPPORTED);
                    //ready = select.select([self.socket], [], [], self.timeout)
                    if (ready[0]) {
                        //pass
                    } else {
                        throw new Error(ErrorCode.CONTACTING_DEVICE);
                    }
                }

                while(true) { // God no, why, only for now.
                    if(this.buffer.slice(0, 1)) {
                        data.write(this.buffer.slice(0, 1));
                        this.buffer = this.buffer.slice(1);
                        pointer++;
                        break;
                    }
                }
            }
            return data;
        }
    }

    teste() {
        return 'teste';
    }
}

exports.ErrorCode = ErrorCode;
exports.createBITalino = function (macAddress, timeout = null, callback) {
    new BITalino(macAddress, timeout, callback);
};