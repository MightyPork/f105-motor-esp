var page_status = (function() {
	var st = {};

	function onUpdate(resp, status) {
		if (status != 200) {
			// bad response
			console.error('Update failed.');
		} else {
			try {
				// OK
				var j = JSON.parse(resp);

				$('.sta-only').toggle(j.sta);
				$('.ap-only').toggle(j.ap);

				$('#uptime').html(j.uptime);
				$('#heap').html(j.heap + " bytes");
				$('#wmode').html(j.wifiMode);

				if (j.sta) {
					$('#staSSID').html(j.sta.SSID);
					$('#staRSSIperc').html(j.sta.RSSIperc);
					$('#staRSSI').html(j.sta.RSSI);
					$('#staMAC').html(j.sta.MAC);
				}

				if (j.ap) {
					$('#apSSID').html(j.ap.SSID);
					$('#apHidden').html(j.ap.hidden ? "Yes" : "No");
					$('#apAuth').html(j.ap.auth);

					// hide the password row if auth is Open
					$('.ap-auth-only').toggle(j.ap.auth != 'Open');

					$('#apPwd').html(j.ap.pwd);
					$('#apChan').html(j.ap.chan);
					$('#apMAC').html(j.ap.MAC);
				}
				// chip ID & macs don't change
			} catch(e) {
				console.error(e);
			}
		}

		setTimeout(requestUpdate, 10000);
	}

	function requestUpdate() {
		$().get(_root+'/api/status.json', onUpdate);
	}

	st.init = function() {
		requestUpdate();
		setTimeout(requestUpdate, 10000);
	};

	return st;
})();
