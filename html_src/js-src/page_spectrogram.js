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

	var colormap = {
		r: [
			{x: 0,  b: 0},
			{x: .7, b: 0},
			{x: 1,  b: 1},
		],
		g: [
			{x: 0,  b: 0},
			{x: .3, b: 0},
			{x: .7, b: 1},
			{x: 1,  b: 1},
		],
		b: [
			{x: 0,   b: 0},
			{x: .02, b: .3},
			{x: .3,  b: 1},
			{x: 1,   b: 1},
		]
	};

	function cmResolv(db, tab) {
		var startX,endX,startC,endC;

		db /=6;
		if (db > 1) db = 1;
		if (db < 0) db = 0;

		for (var i = 0; i < tab.length; i++) {
			var p = tab[i];
			if (db >= p.x) {
				startX = p.x;
				startC = p.b;
			}

			if (db <= p.x) {
				endX = p.x;
				endC = p.b;
			}
		}

		return Math.round((startC + (endC - startC)*((db - startX)/(endX - startX)))*255);
	}

	function val2color(x) {
		var xx = x;//20 * Math.log(x);

		var r = cmResolv(xx, colormap.r);
		var g = cmResolv(xx, colormap.g);
		var b = cmResolv(xx, colormap.b);

		return 'rgb('+r+','+g+','+b+')';
	}

	function shiftSg() {
		var imageData = ctx.getImageData(plot.x+plot.dx, plot.y, plot.w-plot.dx, plot.h);
		ctx.putImageData(imageData, plot.x, plot.y);
	}

	function drawSg(col) {
		shiftSg();

		var bc = opts.sampCount/2;
		for (var i = 0; i < bc; i++) {
			// resolve color from the value
			var y = bc - i - 1;
			var clr;

			if (i > col.length) {
				clr = '#000';
			} else {
				clr = val2color(col[i]);
			}
			ctx.fillStyle = clr;

			ctx.fillRect(plot.x+plot.w-plot.dx, plot.y+y*plot.dy, plot.dx, plot.dy);
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

		ctx.fillStyle = '#000';
		ctx.fillRect(plot.x, plot.y, plot.w, plot.h);

		$('#go-btn').on('click', function() {
			opts.interval = +$('#interval').val(); // ms
			opts.freq = $('#freq').val()*2;
			opts.sampCount = $('#count').val();
			plot.dx = plot.dy = plot.h/(opts.sampCount/2);

			running = !running;
			if (running) {
				requestData();
			} else {
				clearTimeout(readTimeout);
			}

			$('#go-btn')
				.toggleClass('btn-green')
				.toggleClass('btn-red')
				.html(running ? 'Stop' : 'Start');
		});
	};

	return sg;
})();
