/** Wifi page */
var wifi = (function () {
	var wifi = {};
	var authStr = ['Open', 'WEP', 'WPA', 'WPA2', 'WPA/WPA2'];

	/** Update display for received response */
	function onScan(resp, status) {
		if (status != 200) {
			// bad response
			rescan(5000); // wait 5sm then retry
			return;
		}

		resp = JSON.parse(resp);

		var done = !bool(resp.result.inProgress) && (resp.result.APs.length > 0);
		rescan(done ? 15000 : 1000);
		if (!done) return; // no redraw yet

		// clear the AP list
		var $list = $('#ap-list');
		// remove old APs
		$('.AP').remove();

		$list.toggle(done);
		$('#ap-loader').toggle(!done);

		// scan done
		resp.result.APs
			.sort(function (a, b) {
				return b.rssi - a.rssi
			})
			.forEach(function (ap) {
				ap.enc = parseInt(ap.enc);

				if (ap.enc > 4) return; // hide unsupported auths

				var item = document.createElement('div');

				var $item = $(item)
					.data('ssid', ap.essid)
					.data('pwd', ap.enc != 0)
					.addClass('AP');

				// mark current SSID
				if (ap.essid == wifi.current) {
					$item.addClass('selected');
				}

				var inner = document.createElement('div');
				var $inner = $(inner).addClass('inner')
					.htmlAppend('<div class="rssi">{0}</div>'.format(rssiPerc(ap.rssi)))
					.htmlAppend('<div class="essid" title="{0}">{0}</div>'.format(e(ap.essid)))
					.htmlAppend('<div class="auth">{0}</div>'.format(authStr[ap.enc]));

				$item.on('click', function () {
					var $th = $(this);

					// populate the form
					$('#conn-essid').val($th.data('ssid'));
					$('#conn-passwd').val(''); // clear

					if ($th.data('pwd')) {
						// this AP needs a password
						modal.show('#psk-modal');
					} else {
						$('#conn-form').submit();
					}
				});


				item.appendChild(inner);
				$list[0].appendChild(item);
			});
	}

	/** Ask the CGI what APs are visible (async) */
	function scanAPs() {
		$().get('/wifi/scan.cgi', onScan, true, true); // no cache, no jsonp
	}

	function rescan(time) {
		setTimeout(scanAPs, time);
	}

	/** Set up the WiFi page */
	wifi.init = function () {
		//var ap_json = {
		//	"result": {
		//		"inProgress": "0",
		//		"APs": [
		//			{"essid": "Chlivek", "bssid": "88:f7:c7:52:b3:99", "rssi": "204", "enc": "4", "channel": "1"},
		//			{"essid": "TyNikdy", "bssid": "5c:f4:ab:0d:f1:1b", "rssi": "164", "enc": "3", "channel": "1"},
		//			{"essid": "UPC5616805", "bssid": "08:95:2a:0c:84:3f", "rssi": "164", "enc": "4", "channel": "1"},
		//			{"essid": "Sitovina", "bssid": "20:cf:30:98:cb:3a", "rssi": "166", "enc": "3", "channel": "1"},
		//			{"essid": "Tramp", "bssid": "c4:e9:84:6f:6c:e0", "rssi": "170", "enc": "3", "channel": "2"},
		//			{"essid": "KV2", "bssid": "4c:5e:0c:2c:84:9b", "rssi": "172", "enc": "3", "channel": "3"},
		//			{"essid": "UPC373123", "bssid": "e8:40:f2:ae:0e:f4", "rssi": "164", "enc": "3", "channel": "1"},
		//			{"essid": "www.podoli.org prazak", "bssid": "00:00:00:00:00:00", "rssi": "165", "enc": "0", "channel": "4"},
		//			{"essid": "Medvjedov", "bssid": "00:00:00:00:00:00", "rssi": "181", "enc": "4", "channel": "6"},
		//			{"essid": "MARIAN-PC", "bssid": "f8:d1:11:af:d7:72", "rssi": "175", "enc": "3", "channel": "6"},
		//			{"essid": "UPC3226244", "bssid": "64:7c:34:9a:6f:7c", "rssi": "169", "enc": "4", "channel": "6"},
		//			{"essid": "molly", "bssid": "00:00:00:00:00:00", "rssi": "168", "enc": "3", "channel": "7"},
		//			{"essid": "UPC2607759", "bssid": "88:f7:c7:4e:c1:b2", "rssi": "164", "enc": "4", "channel": "8"},
		//			{"essid": "blondyna", "bssid": "98:fc:11:bd:0f:b8", "rssi": "166", "enc": "4", "channel": "9"},
		//			{"essid": "UPC246587811", "bssid": "80:f5:03:20:6c:85", "rssi": "171", "enc": "3", "channel": "11"},
		//			{"essid": "UPC930648", "bssid": "4c:72:b9:50:6d:38", "rssi": "167", "enc": "3", "channel": "11"},
		//			{"essid": "PRAHA4.NET-R21-2", "bssid": "00:00:00:00:00:00", "rssi": "173", "enc": "0", "channel": "12"},
		//			{"essid": "Internet_B0", "bssid": "5c:f4:ab:11:3b:b3", "rssi": "166", "enc": "3", "channel": "13"}
		//		]
		//	}
		//};

		//onScan(ap_json, 200);
		scanAPs();
	};

	return wifi;
})();
