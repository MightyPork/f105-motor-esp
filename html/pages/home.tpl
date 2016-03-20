<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>Home - Current analyzer</title>

	<link href="/css/app.css" rel="stylesheet">

	
	<script src="/js/all.js"></script>
</head>
<body class="page-home">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current analyzer</div>
	<a href="/" class="selected">Home</a><a href="/wifi">WiFi config</a><a href="/fft">FFT</a><a href="/spectrogram">Spectrogram</a><a href="/transient">Power-on transient</a><a href="/about">Credits &amp; About</a></nav>
<div id="content">

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

<script>
	$().ready(initDef);
</script>
</div>
</div>
</body>
</html>
