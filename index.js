/*jshint node:true*/

'use strict';

var RsvgHandle = require('./build/Release/rsvg').RsvgHandle;
var Writable = require('stream').Writable;
var util = require('util');

function Rsvg(buffer) {
	var self = this;
	var options;
	if (Buffer.isBuffer(buffer)) {
		options = {};
	} else if (typeof(buffer) === 'string') {
		buffer = new Buffer(buffer);
		options = {};
	} else if (buffer === undefined || buffer === null) {
		options = {};
	} else {
		throw new TypeError('Invalid argument: buffer');
	}
	Writable.call(self, options);

	self.handle = new RsvgHandle(buffer);
	self.on('finish', function() {
		try {
			self.handle.close();
		} catch (error) {
			self.trigger('error', error);
		}
	});

	Object.defineProperty(self, 'baseURI', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getBaseURI();
		},
		set: function(uri) {
			self.handle.setBaseURI(uri);
		}
	});

	Object.defineProperty(self, 'dpi', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getDPI();
		},
		set: function(dpi) {
			self.handle.setDPI(dpi);
		}
	});

	Object.defineProperty(self, 'dpiX', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getDPIX();
		},
		set: function(dpi) {
			self.handle.setDPIX(dpi);
		}
	});

	Object.defineProperty(self, 'dpiY', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getDPIY();
		},
		set: function(dpi) {
			self.handle.setDPIY(dpi);
		}
	});

	Object.defineProperty(self, 'em', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getEM();
		}
	});

	Object.defineProperty(self, 'ex', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getEX();
		}
	});

	Object.defineProperty(self, 'width', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getWidth();
		}
	});

	Object.defineProperty(self, 'height', {
		configurable: true,
		enumerable: true,
		get: function() {
			return self.handle.getHeight();
		}
	});
}
util.inherits(Rsvg, Writable);

Rsvg.prototype.baseURI = null;
Rsvg.prototype.dpi = 90;
Rsvg.prototype.dpiX = 90;
Rsvg.prototype.dpiY = 90;
Rsvg.prototype.em = 0;
Rsvg.prototype.ex = 0;
Rsvg.prototype.width = 0;
Rsvg.prototype.height = 0;

Rsvg.prototype._write = function(data, encoding, callback) {
	try {
		this.handle.write(data);
	} catch (error) {
		callback(error);
		return;
	}

	callback();
};

Rsvg.prototype.dimensions = function() {
	return this.handle.dimensions();
};

Rsvg.prototype.dimensionsOfElement = function(id) {
	return this.handle.dimensionsOfElement(id);
};

Rsvg.prototype.positionOfElement = function(id) {
	return this.handle.positionOfElement(id);
};

Rsvg.prototype.hasElement = function(id) {
	return this.handle.hasElement(id);
};

Rsvg.prototype.renderRaw = function(format, width, height) {
	return this.handle.renderRaw(format, width, height);
};

Rsvg.prototype.renderPNG = function(width, height) {
	return this.handle.renderPNG(width, height);
};

Rsvg.prototype.renderPDF = function(width, height) {
	return this.handle.renderPDF(width, height);
};

Rsvg.prototype.renderSVG = function(width, height) {
	return this.handle.renderSVG(width, height);
};

Rsvg.prototype.toString = function() {
	var obj = {};

	var baseURI = this.baseURI;
	if (baseURI) {
		obj.baseURI = baseURI;
	}

	var dpiX = this.dpiX;
	var dpiY = this.dpiY;
	if (dpiX === dpiY) {
		obj.dpi = dpiX;
	} else {
		obj.dpiX = dpiX;
		obj.dpiY = dpiY;
	}

	obj.width = this.width;
	obj.height = this.height;

	return '{ [Rsvg]' + util.inspect(obj).slice(1);
};

exports.Rsvg = Rsvg;