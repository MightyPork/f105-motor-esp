<?php $page = 'about'; include "_start.php"; ?>

<h1>About</h1>

<div class="Box">
	<img src="/img/cvut.svg" id="logo" class="mq-tablet-min">
	<h2><?= e($appname) ?></h2>

	<img src="/img/cvut.svg" id="logo2" class="mq-phone">

	<p>&copy; Ondřej Hruška, 2016 &lt;<a href="mailto:ondra@ondrovo.com" target="blank">ondra@ondrovo.com</a>&gt;</p>

	<p><a href="http://measure.feld.cvut.cz/" target="blank">Katedra měření FEL ČVUT</a><br>Department of Measurement, FEE CTU</p>
</div>

<div class="Box">
	<h2>Firmware</h2>

	<p>
		The ESP8266 firmware is based on the amazing <a href="https://github.com/Spritetm/esphttpd" target="blank">esp-httpd</a>
		library by Jeroen Domburg.
	</p>

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
	</table>
</div>

<?php include "_end.php"; ?>