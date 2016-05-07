<?php $page = 'about'; include "_start.php"; ?>

<h1>About</h1>

<div class="Box">
	<img src="/img/cvut.svg" id="logo" class="mq-tablet-min">
	<h2><?= e($appname) ?></h2>

	<img src="/img/cvut.svg" id="logo2" class="mq-phone">

	<p>&copy; Ondřej Hruška, 2016 &lt;<a href="mailto:ondra@ondrovo.com" target="blank">ondra@ondrovo.com</a>&gt;</p>

	<p><a href="http://measure.feld.cvut.cz/" target="blank">Katedra měření, FEL ČVUT</a><br>Department of Measurement, FEE CTU</p>
</div>

<div class="Box">
	<h2>Firmware</h2>

	<table>
		<tr>
			<th>Firmware</th>
			<td>v%vers_fw%, build <i>%date%</i> at <i>%time%</i></td>
		</tr>
		<tr>
			<th>esp-httpd&nbsp;lib</th>
			<td>v%vers_httpd%</td>
		</tr>
		<tr>
			<th>SBMP&nbsp;lib</th>
			<td>v%vers_sbmp%</td>
		</tr>
		<tr>
			<th>ESP&nbsp;IoT&nbsp;SDK</th>
			<td>v%vers_sdk%</td>
		</tr>
		<tr>
			<th>STM32&nbsp;firmware</th>
			<td>v%vers_stm%</td>
		</tr>
	</table>

	<p>
		The webserver was built using the great <a href="https://github.com/Spritetm/esphttpd" target="blank">esp-httpd</a>
		library by Jeroen Domburg.
	</p>
</div>

<?php include "_end.php"; ?>
