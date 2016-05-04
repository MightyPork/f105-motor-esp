<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>About - Current Analyser</title>

	<link href="/css/app.css" rel="stylesheet">
	<script src="/js/all.js"></script>
	<script>
		// server root (or URL) - used for local development with remote AJAX calls
		// (this needs CORS working on the target - which I added to esp-httpd)
		var _root = "";
	</script>
</head>
<body class="page-about">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current Analyser</div>
	<a href="/status">Home</a><a href="/wifi">WiFi config</a><a href="/waveform">Waveform</a><a href="/fft">FFT</a><a href="/spectrogram">Spectrogram</a><a href="/monitoring">Monitoring</a><a href="/about" class="selected">About</a></nav>
<div id="content">
	<img src="/img/loader.gif" alt="Loading…" id="loader">

<h1>About</h1>

<div class="Box">
	<img src="/img/cvut.svg" id="logo" class="mq-tablet-min">
	<h2>Current Analyser</h2>

	<img src="/img/cvut.svg" id="logo2" class="mq-phone">

	<p>&copy; Ondřej Hruška, 2016 &lt;<a href="mailto:ondra@ondrovo.com" target="blank">ondra@ondrovo.com</a>&gt;</p>

	<p><a href="http://measure.feld.cvut.cz/" target="blank">Katedra měření FEL ČVUT</a><br>Department of Measurement, FEE CTU</p>
</div>

<div class="Box">
	<h2>Firmware</h2>

	<table>
		<tr>
			<th>Firmware</th>
			<td>v%vers_fw%, build <i>%date%</i> at <i>%time%</i></td>
		</tr>
		<tr>
			<th>HTTPD</th>
			<td>v%vers_httpd%</td>
		</tr>
		<tr>
			<th>SBMP</th>
			<td>v%vers_sbmp%</td>
		</tr>
		<tr>
			<th>IoT SDK</th>
			<td>v%vers_sdk%</td>
		</tr>
		<!-- Read & show version of the stm32 firmware -->
	</table>

	<p>
		The webserver was built using the great <a href="https://github.com/Spritetm/esphttpd" target="blank">esp-httpd</a>
		library by Jeroen Domburg.
	</p>
</div>


<div class="NotifyMsg hidden" id="notif"></div>

</div><!-- content -->
</div><!-- outer -->
</body>
</html>
