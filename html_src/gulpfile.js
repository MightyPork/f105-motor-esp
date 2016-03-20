//process.env.DISABLE_NOTIFIER = true;

// process.env.NODE_ENV = 'production';

var elixir = require('laravel-elixir');
var gutils = require('gulp-util');

// gutils.env.production = true;

//elixir.config.css.sass.pluginOptions.outputStyle = 'compressed';//gutils.env.production ? 'compressed' : 'expanded';
elixir.config.css.autoprefix.options.browsers = ["last 2 versions", "> 5%"];

elixir.config.assetsPath='.';
elixir.config.publicPath='.';

elixir(function (mix) {
	// Compile CSS
	mix.sass('sass/app.scss', 'css/app.css');

	// JS libs
	mix.scripts([
		'js-src/app.js',
		'js-src/chibi.js'
	], 'js/all.min.js');
});
