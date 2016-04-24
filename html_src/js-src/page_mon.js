var page_mon = (function() {
	var mon = {};

	function updRefInfoField(ok) {
		$('#hasref').html(ok ? 'OK' : 'Not set!');
	}

	/** Capture reference & save to flash */
	mon.captureRef = function() {
		$().get(_root + '/mon/setref', function(resp, status) {
			if (status != 200) {
				// bad response
				errorMsg('Operation failed.');
			} else {
				try {
					// OK
					var j = JSON.parse(resp);
					updRefInfoField(j.success);
				} catch(e) {
					errorMsg(e);
					updRefInfoField(false);
				}
			}
		});
	};

	/** Capture waveform and compare with reference */
	mon.compareNow = function() {
		$().get(_root + '/mon/compare', function(resp, status) {
			if (status != 200) {
				// bad response
				errorMsg('Operation failed.');
			} else {
				try {
					// OK
					var j = JSON.parse(resp);
					if (j.success) {
						$('#actual-dev').html(numfmt(j.deviation, 2));
						$('#actual-rms').html(numfmt(j.rms, 2));
					} else {
						throw 'Capture failed.';
					}
				} catch(e) {
					errorMsg(e);
					$('#actual-dev').html('--');
					$('#actual-rms').html('--');
				}
			}
		});
	};

	function updateXvOnly() {
		// is xively
		var isXv = $('#rep-svc-xv')[0].checked;

		if (isXv) {
			$('.xv-only').removeClass('hidden');
		} else {
			$('.xv-only').addClass('hidden');
		}
	}

	mon.init = function() {
		updateXvOnly();

		$('#rep-svc-xv,#rep-svc-ts').on('change', updateXvOnly);

		setInterval(function() {
			$().get(_root + '/mon/status', function(resp, status) {
				if (status == 200) {
					try {
						// OK
						var j = JSON.parse(resp);
						if (j.success) {
							$('#actual-dev').html(numfmt(j.deviation, 2));
							$('#actual-rms').html(numfmt(j.rms, 2));
						} else {
							console.error('Capture failed.');
						}
					} catch(e) {
						errorMsg(e);
						$('#actual-dev').html('--');
						$('#actual-rms').html('--');
					}
				}
			});
		}, 10000);
	};

	return mon;
})();
