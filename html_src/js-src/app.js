// requires other modules...

// - utils.js
// - modal.js
// - wifi.js

// all must be included after 3rd party libs


/** Global generic init */
$().ready(function () {

	// loader dots...
	setInterval(function () {
		$('.anim-dots').each(function (x) {
			var $x = $(x);
			var dots = $x.html() + '.';
			if (dots.length == 5) dots = '.';
			$x.html(dots);
		});
	}, 1000);

	modal.init();
});
