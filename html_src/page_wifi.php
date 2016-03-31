<?php $page = 'wifi'; include "_start.php"; ?>

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
		<form action="/wifi/connect.cgi" method="post" id="conn-form">
			<input type="hidden" id="conn-essid" name="essid"><!--
			--><label for="psk">Password:</label><!--
			--><input type="password" id="conn-passwd" name="passwd"><!--
			--><input type="submit" value="Connect!">
		</form>
	</div>
</div>

<?php include "_end.php"; ?>
