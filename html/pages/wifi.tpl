<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>
Notice: Undefined index: wifi in /home/ondra/devel/f105motor_esp/html_src/_start.php on line 24
 - WiFi Demo</title>

	<link href="/css/app.css" rel="stylesheet">
	<script src="/js/all.js"></script>
	<script>
		// server root (or URL) - used for local development with remote AJAX calls
		// (this needs CORS working on the target - which I added to esp-httpd)
		var _root = "";
	</script>
</head>
<body class="page-wifi">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">WiFi Demo</div>
	<a href="/home">Home</a></nav>
<div id="content">
	<img src="/img/loader.gif" alt="Loading…" id="loader">

<h1>Wireless Setup</h1>

<div class="Box">
	<table>
		<tr>
			<th>WiFi mode:</th>
			<td>%WiFiMode%</td>
		</tr>
		<tr>
			<th>Note:</th>
			<td>%WiFiapwarn%</td>
		</tr>
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
		<form action="/wifi/connect" method="post" id="conn-form">
			<input type="hidden" id="conn-essid" name="essid"><!--
			--><label for="psk">Password:</label><!--
			--><input type="password" id="conn-passwd" name="passwd"><!--
			--><input type="submit" value="Connect!">
		</form>
	</div>
</div>


<div class="NotifyMsg hidden" id="notif"></div>

</div><!-- content -->
</div><!-- outer -->
</body>
</html>
