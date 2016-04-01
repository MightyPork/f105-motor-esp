<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>Home - Current Analyser</title>

	<link href="/css/app.css" rel="stylesheet">
	<script src="/js/all.js"></script>
	<script>
		// server root (or URL) - used for local development with remote AJAX calls
		// (this needs CORS working on the target - which I added to esp-httpd)
		var _root = "";
	</script>
</head>
<body class="page-home">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current Analyser</div>
	<a href="/status" class="selected">Home</a><a href="/wifi">WiFi config</a><a href="/waveform">Waveform</a><a href="/fft">FFT</a><a href="/spectrogram">Spectrogram</a><a href="/about">About</a></nav>
<div id="content">
	<img src="/img/loader.gif" alt="Loading…" id="loader">

<h1>System Status</h1>

<div class="Box">
	<h2>Runtime</h2>
	<table>
		<tr>
			<th>Uptime:</th>
			<td id="uptime">%uptime%</td>
		</tr>
		<tr>
			<th>Free heap:</th>
			<td id="heap">%heap%</td>
		</tr>
		<tr>
			<th></th>
			<td><a onclick="page_status.trigReset()" class="button btn-red">SW reset</a></td>
		</tr>
	</table>
</div>

<div class="Box">
	<h2>WiFi</h2>
	<table>
		<tr>
			<th>WiFi mode:</th>
			<td id="wmode">%wifiMode%</td>
		</tr>
	</table>
</div>

<!-- WiFi info is read & updated using AJAX -->

<div class="Box sta-only" style="display:none">
	<h2>WiFi Station</h2>
	<table>
		<tr>
			<th>SSID:</th>
			<td id="staSSID"></td>
		</tr>
		<tr>
			<th>RSSI:</th>
			<td>
				<span id="staRSSIperc"></span>,
				<span id="staRSSI"></span>
			</td>
		</tr>
		<tr>
			<th>MAC:</th>
			<td id="staMAC"></td>
		</tr>
	</table>
</div>

<div class="Box ap-only" style="display:none">
	<h2>WiFi AP</h2>
	<table>
		<tr>
			<th>SSID:</th>
			<td id="apSSID"></td>
		</tr>
		<tr>
			<th>Hidden:</th>
			<td id="apHidden"></td>
		</tr>
		<tr>
			<th>Auth. mode:</th>
			<td id="apAuth"></td>
		</tr>
		<tr class="ap-auth-only">
			<th>Password:</th>
			<td id="apPwd"></td>
		</tr>
		<tr>
			<th>Channel:</th>
			<td id="apChan"></td>
		</tr>
		<tr>
			<th>MAC:</th>
			<td id="apMAC"></td>
		</tr>
	</table>
</div>

<div class="Modal hidden no-close" id="reset-modal">
	<div class="Dialog center">
		<h2>The device has been reset.</h2>
		<p class="ap-only">If you're connected to the AP, you'll have to re-connect.</p>
		<p>This dialog should close when the restart is complete, please wait around 15 seconds..</p>
		<p><a onclick="location.reload()" class="button btn-blue">Reload the page</a></p>
	</div>
</div>

<script>
	$().ready(page_status.init);
</script>


<div class="ErrMsg hidden" id="notif"></div>

</div><!-- content -->
</div><!-- outer -->
</body>
</html>
