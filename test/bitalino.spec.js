const assert = require('chai').assert;
const { createBITalino, BITalino, ErrorCode } = require('../index');

const BITALINO_MAC = '98:D3:31:30:26:43';

describe('#createBITalino()', function() {
  it('should throw on invalid MAC', function() {
    assert.throws(() => {
      createBITalino('76-24-B3-28-E9-4Z', null, null);
    }, ErrorCode.INVALID_ADDRESS);
  });

  it('should return an instance of BITalino', function(done) {
    createBITalino(BITALINO_MAC, null, function(bitalino) {
      if (bitalino instanceof BITalino) {
        bitalino.close();
        done();
      }
    });
  });
});

describe('#BITalino.version()', function() {
  it('should return an instance of BITalino', function(done) {
    createBITalino(BITALINO_MAC, null, function(bitalino) {
      const version = bitalino.version();
      if (version instanceof String || typeof(version) === 'string') {
        bitalino.close();
        done();
      } else {
        assert.fail();
      }
    });
  });

  it('should return a string with numbers', function(done) {
    createBITalino(BITALINO_MAC, null, function(bitalino) {
      const version = bitalino.version();
      if (version.search(/\d/) >= 0) {
        bitalino.close();
        done();
      } else {
        assert.fail();
      }
    });
  });

  it('should return a string with numbers after a \'v\'', function(done) {
    createBITalino(BITALINO_MAC, null, function(bitalino) {
      const version = bitalino.version();
      if (version.search(/[vV]\d.\d/) >= 0) {
        bitalino.close();
        done();
      } else {
        assert.fail();
      }
    });
  });
});
