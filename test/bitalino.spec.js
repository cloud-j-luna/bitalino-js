const assert = require('chai').assert;
const { createBITalino, ErrorCode } = require('../index');

describe('#createBITalino()', function () {
    it('should throw on invalid MAC', function () {
        // Arrange

        // Act

        // Assert
        assert.throws(() => {
            createBITalino('76-24-B3-28-E9-4Z', null, null);
        }, ErrorCode.INVALID_ADDRESS);
    });
});