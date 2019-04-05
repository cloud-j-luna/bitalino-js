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
        this.buffer = Buffer.alloc(48);

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
                const bluetooth = require('node-bluetooth');

                bluetooth.connect(address, 1, (err, connection) => {
                    if (err) throw new Error(err);
                    this.socket = connection;

                    console.log(`Connected to ${address}.`)

                    this.socket.on('data', (buffer) => {
                        console.log('from device' + buffer.toString('utf-8'));
                        this.buffer.write(buffer);
                    });

                    setInterval(() => {
                        setupVersion();
                        callback(this);
                    }, 100);
                }, () => {
                    console.log('cannot connect');
                });
                
            } else {
                throw new Error(ErrorCode.INVALID_PLATFORM);
            }
        } else if (address.substr(0, 3) === 'COM' && process.platform === 'win32'
            || address.substr(0, 5) === '/dev/' && process.platform !== 'win32') {   // BLE
            const btSerial = new (require('bluetooth-serial-port')).BluetoothSerialPort();
            btSerial.connect(address, 115200, function () {
                console.log('connected');

                this.socket = btSerial;
            }, function () {
                console.log('cannot connect');
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
            this.split_string = '_v';
            this.split_string_old = 'V';
            const version = this.version();

            console.log('saf');

            if (version.indexOf(this.split_string) >= 0) {
                var versionNumber = Number(version.split(split_string)[1]);
            } else {
                var versionNumber = Number(version.split(split_string_old)[1]);
            }
            this.isBitalino2 = (versionNumber >= 4.2);
        }
    }

    version() {
        if (this.started === false) {
            // CommandVersion: 00000111, 0x07, 7
            this.send(0xe0);
            let versionStr = '';
            while (true) {
                versionStr += this.receive(1);
                if (versionStr[-1] === '\n' && versionStr.indexOf('BITalino') >= 0) {
                    break;
                }
            }
            return versionStr.splice(versionStr.indexOf('BITalino'), -1);
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
                    //if (ready[0]) {
                        //pass
                    //} else {
                    //    throw new Error(ErrorCode.CONTACTING_DEVICE);
                    //}
                }

                let d = null;
                do {
                    d = this.buffer[0];
                    if (d !== 0 && d) {
                        data.write(d, pointer);
                        pointer++;
                        break;
                    }

                    this.buffer = this.buffer.slice(1);
                } while (d === 0);
            }
            return data;
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
        this.socket.write(Buffer.from([data], 'utf-8'), (err) => {
            if(err) throw new Error(err);
            console.log(`Wrote ${data}.`);
        });
    }
}

exports.ErrorCode = ErrorCode;
exports.createBITalino = function (address, timeout = null, callback) {
    new BITalino(address, timeout, callback);
};