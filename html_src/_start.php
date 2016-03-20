<?php

$menu = [
	'home'        => [ '/',            'Home'               ],
	'wifi'        => [ '/wifi',        'WiFi config'        ],
	'fft'         => [ '/fft',         'FFT'                ],
	'spectrogram' => [ '/spectrogram', 'Spectrogram'        ],
	'transient'   => [ '/transient',   'Power-on transient' ],
	'about'       => [ '/about',       'Credits & About'    ],
];

$appname = 'Current analyzer';

?><!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<title><?= htmlspecialchars($menu[$page][1]) ?> - <?= htmlspecialchars($appname) ?></title>

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
	<?php
	// generate the menu
	foreach($menu as $k => $m) {
		$sel = ($page == $k) ? ' class="selected"' : '';
		$text = htmlspecialchars($m[1]);
		echo "<a href=\"$m[0]\"$sel>$text</a>";
	}
	?>
</nav>
<div id="content">
