function bool(x) {
	return (x === 1 || x === '1' || x === true || x === 'true');
}

/**
 * Perform a substitution in the given string.
 *
 * Arguments - array or list of replacements.
 * Arguments numeric keys will replace {0}, {1} etc.
 * Named keys also work, ie. {foo: "bar"} -> replaces {foo} with bar.
 *
 * Braces are added to keys if missing.
 *
 * @returns {String} result
 */
String.prototype.format = function () {
	var out = this;

	var repl = arguments;

	if (arguments.length == 1 && (_.isArray(arguments[0]) || _.isObject(arguments[0]))) {
		repl = arguments[0];
	}

	for (var ph in repl) {
		if (repl.hasOwnProperty(ph)) {
			var ph_orig = ph;

			if (!ph.match(/^\{.*\}$/)) {
				ph = '{' + ph + '}';
			}

			// replace all occurrences
			var pattern = new RegExp(_.escapeRegExp(ph), "g");
			out = out.replace(pattern, repl[ph_orig]);
		}
	}

	return out;
};