var page_spectrogram = (function () {
	var sg = {};

	var ctx;

	// drawing area
	var plot = {
		x:50,
		y:10,
		w:740,//860 total
		h:512,
		dx: 1, // bin
		dy: 1
	};

	var opts = {
		interval: 0,
		sampCount: 0,
		freq:0
	};

	var interval = 1000;
	var running = false;
	var readTimeout; // timer
	var readoutPending;
	var readXhr;

	var lastLoadMs;
	var lastMarkMs;
	var lastMark10s;

	var colormap = [
		/* [val, r, g, b] */
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
	];

	function val2color(val) {
		var x1, x2, c1, c2;

		val = Math.log10(1+val);

		if (val > 2) val = 2;
		if (val < 0) val = 0;

		for (var i = 0; i < colormap.length; i++) {
			var c = colormap[i];
			var point = c[0];
			if (val >= point) {
				x1 = point;
				c1 = c;
			}

			if (val <= point) {
				x2 = point;
				c2 = c;
				break;
			}
		}

		var rate = ((val - x1)/(x2 - x1));
		if (x1 == x2) rate=0;

		var r =	Math.round((c1[1] + (c2[1] - c1[1])*rate));
		var g =	Math.round((c1[2] + (c2[2] - c1[2])*rate));
		var b =	Math.round((c1[3] + (c2[3] - c1[3])*rate));
		return 'rgb('+r+','+g+','+b+')';
	}

	function shiftSg() {
		var imageData = ctx.getImageData(plot.x+plot.dx, plot.y, plot.w-plot.dx, plot.h+6);

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

			var tx = plot.x+plot.w-plot.dx;
			var ty = plot.y+plot.h-(i+1)*plot.dy;
			var tw = plot.dx;
			var th = plot.dy;

			if (ty<plot.y) {
				th -= plot.y-ty;
				ty = plot.y;
			}

			ctx.fillRect(tx, ty, tw, th);
		}

		// mark every 10 s
		console.log('remain',msElapsed(lastMarkMs));
		if (msElapsed(lastMarkMs) >= 950) {
			lastMarkMs = msNow();

			var long = false;
			if (msElapsed(lastMark10s) > 9500) {
				long = true;
				lastMark10s = msNow();
			}

			ctx.strokeStyle = 'white';
			ctx.beginPath();
			ctx.moveTo(plot.x+plot.w-.5, plot.y+plot.h+1);
			ctx.lineTo(plot.x+plot.w-.5, plot.y+plot.h+1+(long?6:2));
			ctx.stroke();
		} else {
			ctx.clearRect(plot.x+plot.w-1, plot.y+plot.h+1,2,10);
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

	function drawLegend() {
		var gap = 8;
		var barW = 10;
		var barH = plot.h-12;
		var barY = plot.y+6;
		var barX = plot.x - gap - barW;
		var vStep = (100 / barH);
		for (var i = 0; i < barH; i++) {
			var c1 = val2color(i * vStep);
			var c2 = val2color((i + 1) * vStep);

			var y = Math.floor(barY + barH - (i + 1));

			var gradient = ctx.createLinearGradient(0, y + 1, 0, y);
			gradient.addColorStop(0, c1);
			gradient.addColorStop(1, c2);
			ctx.fillStyle = gradient;

			ctx.fillRect(barX, y, barW, 1);
		}

		// border
		ctx.strokeStyle = '#000';
		ctx.strokeRect(barX-.5, barY-.5, barW+1, barH+1);

		vStep = (100 / barH);
		ctx.font = '12px sans-serif';
		ctx.fillStyle = 'white';
		ctx.textAlign = 'right';
		for (var i = 0; i <= plot.h; i+=barH/10) {
			ctx.fillText(Math.round(i*vStep)+"", plot.x - gap - barW - gap, barY+barH-i+3);
		}
	}

	function drawAxis() {
		var gap = 8;
		var rX0 = plot.x+plot.w;
		var rX = rX0+gap;
		var rY = plot.y;
		var rH = plot.h;
		var rW = 70;
		ctx.clearRect(rX0+.5, rY-10, rW, rH+20);

		var perBin = (opts.freq/2) / (opts.sampCount/2);

		var totalBins = (plot.h / plot.dy);
		var totalHz = totalBins*perBin;

		console.log("perbin=",perBin,"totalBins=",totalBins,"totalHz=",totalHz);

		var step;

		var steps = [
			[200, 10],
			[1000, 50],
			[3000, 250],
			[5000, 500],
			[10000, 1000],
			[25000, 2500],
			[50000, 5000],
			[100000, 10000],
			[500000, 50000],
			[1000000, 100000],
			[1/0, 250000],
		];
		for(var i = 0; i <= steps.length; i++) {
			if (totalHz <= steps[i][0]) {
				step = steps[i][1];
				break;
			}
		}

		step = step/perBin;

		// every step-th bin has a label
		ctx.font = '12px sans-serif';
		ctx.fillStyle = 'white';
		ctx.strokeStyle = 'white';
		ctx.textAlign = 'left';

		var kilos = totalHz >= 10000;

		// labels and dashes
		for(var i = 0; i <= totalBins+step; i+= step) {
			if (i >= totalBins) {
				var dist = i - totalBins;
				if (dist > step/2) break;// make sure not too close
				i = totalBins;
			}

			var hz = i*(totalHz/totalBins);
			if (hz>=1000000) hz = numfmt(hz/1e6,2)+'M';
			else if (hz>=1000) hz = numfmt(hz/1e3,2)+'k';
			else hz = numfmt(hz,1);

			var yy = Math.round(rY+rH-(plot.dy*i));
			ctx.fillText(hz, rX, yy+4);

			ctx.beginPath();
			ctx.moveTo(rX0, yy+.5);
			ctx.lineTo(rX0+gap/2, yy+.5);
			ctx.stroke();

			if (i >= totalBins) break;
		}

		// Hz label
		ctx.font = '16px sans-serif';
		ctx.save();
		ctx.translate(rX0+50, plot.y+plot.h/2);
		ctx.rotate(Math.PI/2);
		ctx.textAlign = "center";
		ctx.fillText("Frequency - [Hz]", 0, 0);
		ctx.restore();
	}

	function readOpts() {
		opts.interval = +$('#interval').val(); // ms
		opts.freq = +$('#freq').val()*2;
		opts.sampCount = +$('#count').val();

		plot.dx = +$('#tile-x').val();
		plot.dy = +$('#tile-y').val();
	}

	function clearSgArea() {
		ctx.fillStyle = '#000';
		ctx.fillRect(plot.x, plot.y, plot.w, plot.h);
		ctx.strokeStyle = 'white';
		ctx.strokeRect(plot.x-.5, plot.y-.5, plot.w+1, plot.h+1);
	}

	sg.init = function () {
		var canvas = $('#sg')[0];
		ctx = canvas.getContext('2d');

		// CLS
		clearSgArea();
		readOpts();
		drawLegend();
		drawAxis();
		lastMarkMs = msNow()-10000;
		lastMark10s = msNow()-10000;

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
				readOpts();
				drawAxis();

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
