function bool(x) {
	return (x === 1 || x === '1' || x === true || x === 'true');
}

function numfmt(x, places) {
	var pow = Math.pow(10, places);
	return Math.round(x*pow) / pow;
}

function estimateLoadTime(fs, n) {
	return (1000/fs)*n+1500;
}

function msNow() {
	return +(new Date);
}

function msElapsed(start) {
	return msNow() - start;
}

Math.log10 = Math.log10 || function(x) {
	return Math.log(x) / Math.LN10;
};

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

function copyToClipboard(string) {
	if (!document.execCommand) return false;

	// create hidden text element, if it doesn't already exist
	var targetId = "_hiddenCopyText_";

	// must use a temporary form element for the selection and copy
	target = document.getElementById(targetId);
	if (!target) {
		var target = document.createElement("textarea");
		target.style.position = "absolute";
		target.style.left = "-9999px";
		target.style.top = "0";
		target.id = targetId;
		document.body.appendChild(target);
	}
	target.textContent = string;

	// select the content
	var currentFocus = document.activeElement;
	target.focus();
	target.setSelectionRange(0, target.value.length);

	// copy the selection
	var succeed;
	try {
		succeed = document.execCommand("copy");
	} catch(e) {
		succeed = false;
	}
	// restore original focus
	if (currentFocus && typeof currentFocus.focus === "function") {
		currentFocus.focus();
	}

	// clear temporary content
	target.textContent = "";

	return succeed;
}
