var elixir = require('laravel-elixir');
var autoprefixer = require('autoprefixer');

elixir.config.css.autoprefix.options.browsers = ["> 2.5% in CZ"];
elixir.config.css.autoprefix.options.flexbox = "no-2009";

// var info = autoprefixer(elixir.config.css.autoprefix).info();
// console.log(info);


elixir.config.assetsPath='.';
elixir.config.publicPath='.';

elixir(function (mix) {
	// Compile CSS
	mix.sass('sass/app.scss', 'css/app.css');

	// JS libs
	mix.scripts([
		'js-src/app.js',
		'js-src/chibi.js',
		'js-src/chartist.js',
		'js-src/chartist.axis-title.js',
		'js-src/chartist.zoom.js',
	], 'js/all.js');
});
