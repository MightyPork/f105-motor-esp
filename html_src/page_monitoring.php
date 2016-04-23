<?php $page = 'monitoring'; include "_start.php"; ?>

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
			<th>Actual&nbsp;distance:</th>
			<td>
				<span id="reporting" class="Valfield">N/A</span>
				<a onclick="page_mon.compareNow()" class="button btn-blue">Measure</a>
			</td>
		</tr>
	</table>
</div>

<div class="Box">
	<h2>Reporting</h2>
	<form method="POST">
	<table>
		<tr>
			<th><label for="rep-on">Reporting:</label></th>
			<td>
				<input type="checkbox" id="rep-on" name="rep-on" %repEnableCheck%><!--
				-->&nbsp;<label for="rep-on">enabled</label>
			</td>
		</tr>
		<tr>
			<th><label for="rep-interval">Interval:</label></th>
			<td>
				<input type="number" id="rep-interval" style="max-width: 10em" value="%repInterval%">
				seconds
			</td>
		</tr>
		<tr>
			<th>Service:</th>
			<td>
				<input type="radio" name="rep-service" value="xively" id="rep-svc-xv" %repSvcCheckXv%>&nbsp;<label for="rep-svc-xv">Xively</label>&nbsp;
				<input type="radio" name="rep-service" value="thingspeak" id="rep-svc-ts" %repSvcCheckTs%>&nbsp;<label for="rep-svc-ts">ThingSpeak</label>
			</td>
		</tr>
		<tr>
			<th><label for="rep-feed">Feed/Channel:</label></th>
			<td>
				<input type="text" name="rep-feed" id="rep-feed" value="%repFeed%">
			</td>
		</tr>
		<tr>
			<th><label for="rep-key">API key:</label></th>
			<td>
				<input type="text" name="rep-key" id="rep-key" value="%repKey%">
			</td>
		</tr>
		<tr>
			<th>&nbsp;</th>
			<td>
				<input type="submit" value="Save changes">
			</td>
		</tr>
	</table>
	</form>
</div>

<script>
	$().ready(page_mon.init);
</script>

<?php include "_end.php"; ?>
