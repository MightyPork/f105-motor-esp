var notify = (function () {
	var nt = {};
	var sel = '#notif';

	nt.show = function (message, timeout) {
		$(sel).html(message);
		modal.show(sel);

		if (!_.isUndefined(timeout)) {
			setTimeout(nt.hide, timeout);
		}
	};

	nt.hide = function () {
		var $m = $(sel);
		$m.removeClass('visible');
		setTimeout(function () {
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
