<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title>Home - WiFi Demo</title>

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
	<div id="brand" onclick="$('#menu').toggleClass('expanded')">WiFi Demo</div>
	<a href="/home" class="selected">Home</a><a href="/wifi">WiFi config</a></nav>
<div id="content">
	<img src="/img/loader.gif" alt="Loading…" id="loader">

<h1>Motor Control</h1>

<div class="Box center">
	<a class="button btn-blue" id="btn-rev">REV</a><span style="display:inline-block;width:2em"></span>
	<a class="button btn-blue" id="btn-fwd">FWD</a>
</div>

<script>
	$().ready(page_home.init);
</script>


<div class="NotifyMsg hidden" id="notif"></div>

</div><!-- content -->
</div><!-- outer -->
</body>
</html>
