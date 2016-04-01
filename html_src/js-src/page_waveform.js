var page_waveform = (function () {
	var wfm = {};

	var zoomResetFn;
	var dataFormat;

	var readoutPending = false;
	var autoReload = false;
	var autoReloadTime = 1;
	var arTimeout = -1;

	var zoomSavedX, zoomSavedY;

	function buildChart(j) {
		// Build the chart
		var mql = window.matchMedia('screen and (min-width: 544px)');
		var isPhone = !mql.matches;

		var fft = (j.stats.format == 'FFT');

		var xLabel, yLabel;
		if (fft) {
			xLabel = 'Frequency - [ Hz ]';
			yLabel = 'Magnitude - [ mA ]';
		} else {
			xLabel = 'Sample time - [ ms ]';
			yLabel = 'Current - [ mA ]';
		}

		var peak = Math.max(-j.stats.min, j.stats.max);
		var displayPeak = Math.max(peak, 10);

		// Sidebar

		$('#stat-count').html(j.stats.count);
		$('#stat-f-s').html(numfmt(j.stats.freq, 2));
		$('#stat-i-peak').html(numfmt(peak, 2));
		$('#stat-i-rms').html(numfmt(j.stats.rms, 2));
		$('.stats').removeClass('invis');

		// --- chart ---

		// Generate point entries
		// add synthetic properties
		var step = fft ? (j.stats.freq/j.stats.count) : (1000/j.stats.freq);
		var points = _.map(j.samples, function (a, i) {
			return {
				x: i * step,
				y: a
			};
		});

		var plugins = [
			Chartist.plugins.zoom({
				resetOnRightMouseBtn: true,
				onZoom: function (chart, reset) {
					zoomResetFn = reset;

					zoomSavedX = chart.options.axisX.highLow;
					zoomSavedY = chart.options.axisY.highLow;
				}
			})
		];

		if (!isPhone) plugins.push( // larger than phone
			Chartist.plugins.ctAxisTitle({
				axisX: {
					axisTitle: xLabel,
					offset: {
						x: 0,
						y: 55
					}
				},
				axisY: {
					axisTitle: yLabel,
					flipText: true,
					offset: {
						x: 0,
						y: 15
					}
				}
			})
		);

		var xHigh, xLow, yHigh, yLow;

		if (zoomSavedX) {
			// we have saved coords of the zoom rect, restore the zoom.
			xHigh = zoomSavedX.high;
			xLow = zoomSavedX.low;
			yHigh = zoomSavedY.high;
			yLow = zoomSavedY.low;
		} else {
			yHigh = fft ? undefined : displayPeak;
			yLow = fft ? 0 : -displayPeak;
		}

		new Chartist.Line('#chart', {
			series: [
				{
					name: 'a',
					data: points
				},
			]
		}, {
			showPoint: false,
			showArea: fft,
			fullWidth: true,
			chartPadding: (isPhone ? {right: 20, bottom: 5, left: 0} : {right: 25, bottom: 30, left: 25}),
			series: {
				'a': {
					lineSmooth: Chartist.Interpolation.monotoneCubic()
				}
			},
			axisX: {
				type: Chartist.AutoScaleAxis,
				//onlyInteger: !fft // only for raw
				high: xHigh,
				low: xLow,
			},
			axisY: {
				type: Chartist.AutoScaleAxis,
				//onlyInteger: true
				high: yHigh,
				low: yLow,
			},
			explicitBounds: {
				xLow: 0,
				yLow: fft ? 0 : undefined,
				xHigh: points[points.length-1].x
			},
			plugins: plugins
		});
	}

	function onRxData(resp, status) {
		readoutPending = false;

		if (status != 200) {
			errorMsg("Request failed.");

			if (autoReload)
				toggleAutoReload(); // turn it off.

		} else {
			var j = JSON.parse(resp);
			if (!j.success) {
				errorMsg("Sampling / readout failed.");

				if (autoReload)
					toggleAutoReload(); // turn it off.

				return;
			}

			buildChart(j);

			if (autoReload)
				arTimeout = setTimeout(requestReload, autoReloadTime);
		}
	}

	function requestReload() {
		if (readoutPending) return false;

		readoutPending = true;

		var n = $('#count').val();
		var fs = $('#freq').val();

		var url = _root+'/measure/'+dataFormat+'?n='+n+'&fs='+fs;
		$().get(url, onRxData, estimateLoadTime(fs,n));

		return true;
	}

	function toggleAutoReload() {
		autoReloadTime = +$('#ar-time').val() * 1000; // ms

		autoReload = !autoReload;
		if (autoReload) {
			requestReload();
		} else {
			clearTimeout(arTimeout);
		}

		$('#ar-btn')
			.toggleClass('btn-blue')
			.toggleClass('btn-red')
			.val(autoReload ? 'Stop' : 'Auto');
	}

	wfm.init = function (format) {
		// --- Load data ---
		dataFormat = format;

		// initial
//		requestReload();

		$('#load').on('click', requestReload);
		$('#count,#freq').on('keyup', function (e) {
			if (e.which == 13) {
				requestReload();
			}
		});

		// --- zooming ---

		$('#chart').on('contextmenu', function (e) { // right click on the chart -> reset
			zoomResetFn && zoomResetFn();
			zoomResetFn = null;

			zoomSavedX = null;
			zoomSavedY = null;

			e.preventDefault();
			return false;
		});

		// auto-reload button
		$('#ar-btn').on('click', toggleAutoReload);
	};

	return wfm;
})();
