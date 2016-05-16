/** Wifi page */
var page_home = (function () {
	var home = {};

	/** Set up the Home page */
	home.init = function () {
		$('#btn-fwd').on('mousedown', function() {
			console.log('FWD down');
			$().get(_root+'/m/start?dir=f');
		}).on('mouseup', function() {
			console.log('FWD up');
			$().get(_root+'/m/stop');
		});

		$('#btn-rev').on('mousedown', function() {
			console.log('REV down');
			$().get(_root+'/m/start?dir=r');
		}).on('mouseup', function() {
			console.log('REV up');
			$().get(_root+'/m/stop');
		});
	};

	return home;
})();
