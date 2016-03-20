<?php $page='home'; include "_start.php"; ?>

<h1>System Status</h1>

<div class="Box">
	<h2>Runtime</h2>
	<table>
		<tbody>
		<tr>
			<th>Uptime:</th>
			<td>%uptime%</td>
		</tr>
		<tr>
			<th>Free heap:</th>
			<td>%heap% bytes</td>
		</tr>
		</tbody>
	</table>
</div>

<div class="Box">
	<h2>Wireless</h2>
	<table>
		<tbody>
		<tr>
			<th>WiFi mode:</th>
			<td>%wifiMode%</td>
		</tr>
		<tr class="sta-only">
			<th>SSID:</th>
			<td>%staSSID%</td>
		</tr>
		<tr class="sta-only">
			<th>RSSI:</th>
			<td><span id="rssi-perc"></span>, <span id="rssi-dbm"></span></td>
		</tr>
		<tr>
			<th>Client MAC:</th>
			<td>%staMAC%</td>
		</tr>
		<tr>
			<th>AP MAC:</th>
			<td>%apMAC%</td>
		</tr>
		</tbody>
	</table>
</div>

<div class="Box">
	<h2>Hardware</h2>
	<table>
		<tbody>
		<tr>
			<th>ESP8266 S/N:</th>
			<td>%chipID%</td>
		</tr>
		</tbody>
	</table>
</div>

<script>
	var wifiMode = '%wifiMode%';
	var staRSSI = '%staRSSI%';

	$().ready(function() {
		if (wifiMode == 'SoftAP') {
			$('.sta-only').hide();
		} else {
			$('#rssi-perc').html(rssiPerc(staRSSI));
			$('#rssi-dbm').html(staRSSI);
		}

		setTimeout(function(){location.reload()}, 10000);
	});
</script>

<?php include "_end.php"; ?>
