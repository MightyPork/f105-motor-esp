var page_spectrogram = (function () {
	var sg = {};

	var ctx;

	// drawing area
	var plot = {
		x:0,
		y:0,
		w:860,
		h:512,
		dx: 1, // bin
		dy: 1
	};

	var opts = {
		interval: 0,
		sampCount: 0,
		binCount: 0,
		freq:0
	};

	var interval = 1000;
	var running = false;
	var readTimeout; // timer
	var readoutPending;
	var readXhr;

	var lastLoadMs;

	var colormap = [
		[0.00, 0, 0, 0],
		[0.10, 41, 17, 41],
		[0.25, 34, 17, 78],
		[0.6, 17, 30, 105],
		[1.0, 17, 57, 126],
		[1.2, 17, 84, 128],
		[1.3, 17, 111, 115],
		[1.4, 17, 134, 96],
		[1.5, 17, 155, 71],
		[1.6, 68, 194, 17],
		[1.75, 111, 209, 17],
		[1.84, 180, 213, 17],
		[1.90, 223, 217, 86],
		[1.97, 248, 222, 176],
		[1.99, 255, 237, 222],
		[2.00, 255, 255, 255],


		// [0.00, 0, 0, 0],
		// [0.12, 41, 17, 41],
		// [0.25, 34, 17, 78],
		// [0.38, 17, 30, 105],
		// [0.50, 17, 57, 126],
		// [0.62, 17, 84, 128],
		// [0.75, 17, 111, 115],
		// [0.88, 17, 134, 96],
		// [1.00, 17, 155, 71],
		// [1.12, 68, 194, 17],
		// [1.25, 111, 209, 17],
		// [1.38, 180, 213, 17],
		// [1.50, 223, 217, 86],
		// [1.62, 248, 222, 176],
		// [1.75, 255, 237, 222],
		// [2.00, 255, 255, 255],
	];

	function cmResolv(val) {
		var x1, x2, c1, c2;

		val = Math.log10(1+val);

		if (val > 2) val = 2;
		if (val < 0) val = 0;

		_.each(colormap, function(c) {
			var point = c[0];
			if (val >= point) {
				x1 = point;
				c1 = c;
			}

			if (val <= point) {
				x2 = point;
				c2 = c;
				return false; // exit iteration
			}
		});

		var rate = ((val - x1)/(x2 - x1));
		if (x1 == x2) rate=0;

		return [
			Math.round((c1[1] + (c2[1] - c1[1])*rate)),
			Math.round((c1[2] + (c2[2] - c1[2])*rate)),
			Math.round((c1[3] + (c2[3] - c1[3])*rate)),
		];
	}

	function val2color(x) {
		var c = cmResolv(x);

		return 'rgb('+c[0]+','+c[1]+','+c[2]+')';
	}

	function shiftSg() {
		var imageData = ctx.getImageData(plot.x+plot.dx, plot.y, plot.w-plot.dx, plot.h);

		ctx.fillStyle = 'black';
		ctx.fillRect(plot.x, plot.y, plot.w, plot.h);

		ctx.putImageData(imageData, plot.x, plot.y);
	}

	function drawSg(col) {
		shiftSg();

		var bc = opts.sampCount/2;
		for (var i = 0; i < bc; i++) {
			// resolve color from the value
			var clr;

			if (i*plot.dy > plot.h) {
				break;
			}

			if (i > col.length) {
				clr = '#000';
			} else {
				clr = val2color(col[i]);
			}
			ctx.fillStyle = clr;

			ctx.fillRect(plot.x+plot.w-plot.dx, plot.y+plot.h-(i+1)*plot.dy, plot.dx, plot.dy);
		}
	}


	function onRxData(resp, status) {
		readoutPending = false;
		if (status == 200) {
			try {
				var j = JSON.parse(resp);
				if (j.success) {
					// display
					drawSg(j.samples);
				} else {
					errorMsg("Sampling failed.", 1000);
				}
			} catch(e) {
				errorMsg(e);
			}
		} else {
			errorMsg("Request failed.", 1000);
		}

		if (running)
			readTimeout = setTimeout(requestData, Math.max(0, opts.interval - msElapsed(lastLoadMs))); // TODO should actually compute time remaining, this adds interval to the request time.
	}

	function requestData() {
		if (readoutPending) {
			errorMsg("Request already pending - aborting.");
			readXhr.abort();
		}
		readoutPending = true;
		lastLoadMs = msNow();

		var fs = opts.freq;
		var n = opts.sampCount;
		var url = _root+'/measure/fft?n='+n+'&fs='+fs;

		readXhr = $().get(url, onRxData, estimateLoadTime(fs,n));

		return true;
	}

	sg.init = function () {
		var canvas = $('#sg')[0];
		ctx = canvas.getContext('2d');

		// CLS
		ctx.fillStyle = '#000';
		ctx.fillRect(plot.x, plot.y, plot.w, plot.h);

		// update tile size on bin count selection
		$('#count').on('change', function() {
			var count = +$('#count').val();
			var tile = Math.max(1, plot.h/(count/2));

			$('#tile-x').val(tile);
			$('#tile-y').val(tile);
		});

		// chain Y with X
		$('#tile-y').on('change', function() {
			$('#tile-x').val($(this).val());
		});

		$('#go-btn').on('click', function() {
			running = !running;
			if (running) {
				opts.interval = +$('#interval').val(); // ms
				opts.freq = +$('#freq').val()*2;
				opts.sampCount = +$('#count').val();

				plot.dx = +$('#tile-x').val();
				plot.dy = +$('#tile-y').val();

				requestData();
			} else {
				clearTimeout(readTimeout);
			}

			$('#go-btn')
				.toggleClass('btn-green')
				.toggleClass('btn-red')
				.html(running ? 'Stop' : 'Start');
		});

		for (var i = 0; i < 99; i++) {
			ctx.fillStyle = val2color(i);
			console.log(ctx.fillStyle);
			ctx.fillRect(i*5,0,5,100);
		}
	};

	return sg;
})();
