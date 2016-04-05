var notify = (function () {
	var nt = {};
	var sel = '#notif';

	var hideTmeo1;
	var hideTmeo2;

	nt.show = function (message, timeout) {
		$(sel).html(message);
		modal.show(sel);

		clearTimeout(hideTmeo1);
		clearTimeout(hideTmeo2);

		if (!_.isUndefined(timeout)) {
			hideTmeo1 = setTimeout(nt.hide, timeout);
		}
	};

	nt.hide = function () {
		var $m = $(sel);
		$m.removeClass('visible');
		hideTmeo2 = setTimeout(function () {
			$m.addClass('hidden');
		}, 250); // transition time
	};

	nt.init = function() {
		$(sel).on('click', function() {
			nt.hide(this);
		});
	};

	return nt;
})();
