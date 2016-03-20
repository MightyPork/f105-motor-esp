<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>WiFi config - Current analyzer</title>

	<link href="/css/app.css" rel="stylesheet">

	<!-- IE8 support (not tested) -->
	<!--[if lt IE 9]>
	<script src="//cdnjs.cloudflare.com/ajax/libs/es5-shim/4.5.7/es5-shim.min.js"></script>
	<script src="//cdnjs.cloudflare.com/ajax/libs/html5shiv/3.7.3/html5shiv.min.js"></script>
	<script src="//cdnjs.cloudflare.com/ajax/libs/respond.js/1.4.2/respond.min.js"></script>
	<![endif]-->

	<script src="/js/all.min.js"></script>
</head>
<body>
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current analyzer</div>
	<a href="/">Home</a><a href="/wifi" class="selected">WiFi config</a><a href="/fft">FFT</a><a href="/spectrogram">Spectrogram</a><a href="/transient">Power-on transient</a><a href="/about">Credits &amp; About</a></nav>
<div id="content">

<h1>Wireless Setup</h1>

<div class="Box">
	<table>
		<tbody>
		<tr>
			<th>WiFi mode:</th>
			<td>%WiFiMode%</td>
		</tr>
		<tr>
			<th>Note:</th>
			<td>%WiFiapwarn%</td>
		</tr>
		</tbody>
	</table>

</div>

<div class="Box" id="ap-box">
	<h2>Select AP to join</h2>
	<div id="ap-loader">Scanning<span class="anim-dots">.</span></div>
	<div id="ap-list" style="display:none"></div>
</div>

<script>
	// Current SSID
	wifi.current = '%currSsid%';
	$().ready(wifi.init);
</script>

<div class="Modal hidden" id="psk-modal">
	<div class="Dialog">
		<form action="/wifi/connect.cgi" method="post" id="conn-form">
			<input type="hidden" id="conn-essid" name="essid"><!--
			--><label for="psk">Password:</label><!--
			--><input type="password" id="conn-passwd" name="passwd"><!--
			--><input type="submit" value="Connect!">
		</form>
	</div>
</div>

<script>
	$().ready(initDef);
</script>
</div>
</div>
</body>
</html>
