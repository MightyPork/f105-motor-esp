<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>WiFi config - Current analyzer</title>

	<link href="/css/app.css" rel="stylesheet">

	
	<script src="/js/all.js"></script>
</head>
<body class="page-wifi">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">Current analyzer</div>
	<a href="/">Home</a><a href="/wifi" class="selected">WiFi config</a><a href="/waveform">Waveform</a></nav>
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
	page_wifi.current = '%currSsid%';
	$().ready(page_wifi.init);
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

</div>
</div>
</body>
</html>
