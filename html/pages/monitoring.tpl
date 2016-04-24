<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>Monitoring - Current Analyser</title>

	<link href="/css/app.css" rel="stylesheet">
	<script src="/js/all.js"></script>
	<script>
		// server root (or URL) - used for local development with remote AJAX calls
		// (this needs CORS working on the target - which I added to esp-httpd)
		var _root = "";
	</script>
</head>
<body class="page-monitoring">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current Analyser</div>
	<a href="/status">Home</a><a href="/wifi">WiFi config</a><a href="/waveform">Waveform</a><a href="/fft">FFT</a><a href="/spectrogram">Spectrogram</a><a href="/monitoring" class="selected">Monitoring</a><a href="/about">About</a></nav>
<div id="content">
	<img src="/img/loader.gif" alt="Loading…" id="loader">

<h1>Monitoring &amp; Reporting</h1>

<div class="Box">
	<h2>Status</h2>
	<table>
		<tr>
			<th>Reference:</th>
			<td>
				<span id="hasref" class="Valfield">%refStored%</span>
				<a onclick="page_mon.captureRef()" class="button btn-green">Capture</a>
			</td>
		</tr>
		<tr>
			<th>Status:</th>
			<td>
				<span class="Valfield" style="vertical-align:middle;">
					Δ = <span id="actual-dev">%curDeviation%</span><br>
					I<sub>RMS</sub> = <span id="actual-rms">%curRMS%</span>&nbsp;mA
				</span>
				<a onclick="page_mon.compareNow()" class="button btn-blue">Measure</a>
			</td>
		</tr>
	</table>
</div>

<div class="Box">
	<h2>Reporting</h2>
	<form action="/mon/config" method="POST">
	<table>
		<tr>
			<th><label for="rep-on">Reporting:</label></th>
			<td>
				<input type="checkbox" id="rep-on" name="enabled" value="1" %rep_en%><!--
				-->&nbsp;<label for="rep-on">enabled</label>
			</td>
		</tr>
		<tr>
			<th><label for="rep-interval">Interval:</label></th>
			<td>
				<input type="number" name="interval" id="rep-interval" style="max-width: 10em" value="%repInterval%"><!--
				-->&nbsp;seconds
			</td>
		</tr>
		<tr>
			<th>Service:</th>
			<td>
				<input type="radio" name="service" value="xv" id="rep-svc-xv" %svc_xv%>&nbsp;<label for="rep-svc-xv">Xively</label>&nbsp;
				<input type="radio" name="service" value="ts" id="rep-svc-ts" %svc_ts%>&nbsp;<label for="rep-svc-ts">ThingSpeak</label>
			</td>
		</tr>
		<tr>
			<th><label for="rep-feed">Feed/Channel:</label></th>
			<td><input type="text" name="feed" id="rep-feed" value="%repFeed%"></td>
		</tr>
		<tr>
			<th><label for="rep-key">API key:</label></th>
			<td><input type="text" name="key" id="rep-key" value="%repKey%"></td>
		</tr>
		<tr>
			<th>&nbsp;</th>
			<td><input type="submit" value="Save changes"></td>
		</tr>
	</table>
	</form>
</div>

<script>
	$().ready(page_mon.init);
</script>


<div class="ErrMsg hidden" id="notif"></div>

</div><!-- content -->
</div><!-- outer -->
</body>
</html>
