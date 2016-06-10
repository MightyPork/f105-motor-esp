/** Wifi page */
var page_home = (function () {
	var home = {};

	var last_slider_val = 0;

	function applySlider() {
		var value = $('#control-range').val();
		if (value == last_slider_val) return;
		last_slider_val = value;

		var dir = 'f';
		if (value < 0) {
			value *= -1;
			dir = 'r';
		}

		value = (6144 * 3) * (value / 100);

		if (value < 20) {
			$().get(_root + '/m/stop');
		} else {
			$().get(_root + '/m/start?dir=' + dir + '&speed=' + Math.round(value));
		}
	}

	function periodicCheckSlider() {
		applySlider();

		setTimeout(periodicCheckSlider, 150);
	}

	/** Set up the Home page */
	home.init = function () {
		$('#btn-stop').on('mousedown', function () {
			$().get(_root + '/m/stop');
			$('#control-range').val(0);
		});

		$('#btn-rev').on('mousedown', function () {
			var rng = $('#control-range');
			var value = 1*rng.val();
			rng.val(value - 10);
			applySlider();
		});

		$('#btn-fwd').on('mousedown', function () {
			var rng = $('#control-range');
			var value = 1*rng.val();
			rng.val(value + 10);
			applySlider();
		});

		$('#control-range').on('change', applySlider);

		periodicCheckSlider();
	};

	return home;
})();
