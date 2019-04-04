const osc = require('osc');
const EventEmitter = require('events');

const ErrorCode = {
    INVALID_PACKET: 'Invalid packet received: '
};

module.exports = class BITalinoRIOT extends EventEmitter {
    constructor() {
        super();
        this.udpPort = new osc.UDPPort({
            localAddress: "0.0.0.0",
            localPort: 8888,
            metadata: true
        });
        
        this.udpPort.on('message', (packet) => {
            if(/^\/[0-9]*\/bitalino/.test(packet.address)) {
        
            } else if(/^\/[0-9]*\/raw/.test(packet.address)) {
                const raw = {
                    acX: packet.args[0],
                    acY: packet.args[1],
                    acZ: packet.args[2],
                    gyroX: packet.args[3],
                    gyroY: packet.args[4],
                    gyroZ: packet.args[5],
                    magnetX: packet.args[6],
                    magnetY: packet.args[7],
                    magnetZ: packet.args[8],
                    temp: packet.args[9],
                    switch: !!packet.args[10],
                    analogInputGPIO3: packet.args[11],
                    analogInputGPIO4: packet.args[12],
                    quaternions: [
                        packet.args[13],
                        packet.args[14],
                        packet.args[15],
                        packet.args[16]
                    ],
                    euler: [
                        packet.args[17],
                        packet.args[18],
                        packet.args[19],
                        packet.args[20]
                    ]
                }

                this.emit('raw', raw);
            } else {
                throw new Error(ErrorCode.INVALID_PACKET + packet);
            }
        });
        
        this.udpPort.open();
        
        
        this.udpPort.on('ready', () => {
            this.emit('ready');
        });
    }
}