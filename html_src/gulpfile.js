var elixir = require('laravel-elixir');
var autoprefixer = require('autoprefixer');

elixir.config.css.autoprefix.options.browsers = ["> 2.5% in CZ"];
elixir.config.css.autoprefix.options.flexbox = "no-2009";
elixir.config.css.cssnano.pluginOptions.discardComments = {removeAll: true};
elixir.config.css.cssnano.pluginOptions.colormin = false;
elixir.config.js.uglify.options.compress.drop_console = false;

// var info = autoprefixer(elixir.config.css.autoprefix).info();
// console.log(info);


elixir.config.assetsPath='.';
elixir.config.publicPath='.';

elixir(function (mix) {
	// Compile CSS
	mix.sass('sass/app.scss', 'css/app.css');

	// JS libs
	mix.scripts([
		'js-src/lib/chibi.js',
		'js-src/lib/chartist.js',
		'js-src/lib/chartist.axis-title.js',
		'js-src/lib/chartist.zoom.js',
		'js-src/lib/lodash.custom.js',
		'js-src/utils.js',
		'js-src/modal.js',
		'js-src/app.js',
		'js-src/page_wifi.js',
		'js-src/page_waveform.js',
		'js-src/page_status.js',
	], 'js/all.js');
});
