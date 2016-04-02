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

	$('input[type=number]').on('mousewheel', function(e) {
		var val = +$(this).val();
		var step = +($(this).attr('step') || 1);
		var min = $(this).attr('min');
		var max = $(this).attr('max');
		if(e.wheelDelta > 0) {
			val += step;
		} else {
			val -= step;
		}
		if (!_.isUndefined(min)) val = Math.max(val, min);
		if (!_.isUndefined(max)) val = Math.min(val, max);
		$(this).val(val);

		e.preventDefault();
	});

	modal.init();
	notify.init();
});


function errorMsg(msg, time) {
	notify.show(msg, time || 3000);
}
