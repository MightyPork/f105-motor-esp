var page_status = (function() {
	var st = {};
	st.j = {};

	var updateTime = 10000;

	var updateInhibited = false;

	st.trigReset = function() {
		var modal_sel = '#reset-modal';
		$().get(_root + '/reset.cgi', function(resp, status) {
			if (status == 200) {

				modal.show(modal_sel);
				updateInhibited = true;

				var ping_i = setInterval(function() {
					$().get(_root+'/ping.cgi', function(resp, code){
						if (code == 200) {
							// device is ready
							modal.hide(modal_sel);
							requestUpdate();
							clearInterval(ping_i);
							updateInhibited = false;
						}
					}, {timeout: 500});
				}, 1000);
			}
		});
	};

	function onUpdate(resp, status) {
		if (status != 200) {
			// bad response
			errorMsg('Update failed.');
		} else {
			try {
				// OK
				var j = JSON.parse(resp);
				st.j = j; // store for global access

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
				errorMsg(e);
			}
		}

		if (!updateInhibited) {
			setTimeout(requestUpdate, updateTime);
		}
	}

	function requestUpdate() {
		$().get(_root+'/api/status.json', onUpdate);
	}

	st.init = function() {
		requestUpdate();
	};

	return st;
})();
