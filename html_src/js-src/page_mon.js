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
						$('#refdist').html(numfmt(j.deviation, 2));
					} else {
						errorMsg('Capture failed.');
						$('#refdist').html('--');
					}
				} catch(e) {
					errorMsg(e);
					$('#refdist').html('--');
				}
			}
		});
	};

	mon.init = function() {
		//
	};

	return mon;
})();
