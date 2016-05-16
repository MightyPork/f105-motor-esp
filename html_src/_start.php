<?php

	$prod = defined('STDIN');
	$root = $prod ? '' : 'http://192.168.1.15';

	$menu = [
		'home'        => [ $prod ? '/home' : '/page_home.php', 'Home'            ],
		'wifi'        => [ $prod ? '/wifi' : '/page_wifi.php',   'WiFi config'   ],
	];

	$appname = 'WiFi Demo';

	function e($s) {
		return htmlspecialchars($s, ENT_HTML5|ENT_QUOTES);
	}

?><!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title><?= e($menu[$page][1]) ?> - <?= e($appname) ?></title>

	<link href="/css/app.css" rel="stylesheet">
	<script src="/js/all.js"></script>
	<script>
		// server root (or URL) - used for local development with remote AJAX calls
		// (this needs CORS working on the target - which I added to esp-httpd)
		var _root = <?= json_encode($root) ?>;
	</script>
</head>
<body class="page-<?=$page?>">
<div id="outer">
<nav id="menu">
	<div id="brand" onclick="$('#menu').toggleClass('expanded')"><?= e($appname) ?></div>
	<?php
	// generate the menu
	foreach($menu as $k => $m) {
		$sel = ($page == $k) ? ' class="selected"' : '';
		$text = e($m[1]);
		$url = e($m[0]);
		echo "<a href=\"$url\"$sel>$text</a>";
	}
	?>
</nav>
<div id="content">
	<img src="/img/loader.gif" alt="Loading…" id="loader">
