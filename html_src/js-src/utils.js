function bool(x) {
	return (x === 1 || x === '1' || x === true || x === 'true');
}

/** html entities */
function e(x) {
	return String(x)
		.replace(/&/g, '&amp;')
		.replace(/"/g, '&quot;')
		.replace(/'/g, '&#39;')
		.replace(/</g, '&lt;')
		.replace(/>/g, '&gt;');
}

/** Returns true if argument is array [] */
function isArray(obj) {
	return Object.prototype.toString.call(obj) === '[object Array]';
}

/** Returns true if argument is object {} */
function isObject(obj) {
	return Object.prototype.toString.call(obj) === '[object Object]';
}

/** escape a string to have no special meaning in regex */
function regexEscape(s) {
	return s.replace(/[-\/\\^$*+?.()|[\]{}]/g, '\\$&');
}

/** Perform a substitution in the given string.
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

	if (arguments.length == 1 && (isArray(arguments[0]) || isObject(arguments[0]))) {
		repl = arguments[0];
	}

	for (var ph in repl) {
		if (repl.hasOwnProperty(ph)) {
			var ph_orig = ph;

			if (!ph.match(/^\{.*\}$/)) {
				ph = '{' + ph + '}';
			}

			// replace all occurrences
			var pattern = new RegExp(regexEscape(ph), "g");
			out = out.replace(pattern, repl[ph_orig]);
		}
	}

	return out;
};
