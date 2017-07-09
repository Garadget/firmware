var s_result = '../webconfig.cpp';

var a_htmlFiles = [
	{ source: 'config.html', regexp: /^const char s_configHtml\[\] = \"(.*)\";/ }
];

var a_cssFiles = [
	{ source: 'config.css', regexp: /^const char s_configCss\[\] = \"(.*)\";/ }
];

var a_jsFiles = [
	{ source: 'config.js', regexp: /^const char s_configJs\[\] = \"(.*)\";/ }
];


function f_injectResult(s_code, r_regexp, a_result) {
	var n_line, a_match;
	for (n_line = 0; n_line < a_result.length; n_line++) {
		a_match = r_regexp.exec(a_result[n_line]);
		if (!a_match)
			continue;
		a_result[n_line] = a_result[n_line].replace(a_match[1], s_code);
	}
	return a_result;
}

function f_saveResult(a_result) {
	o_fileSystem.writeFile(s_result, a_result.join("\n"), function(s_error) {
		if (s_error)
			return console.log(s_error);

		console.log("Saved!");
	});
}

var o_minify = require('html-minifier').minify;
var o_fileSystem = require('fs');
var o_cleanCSS = require('clean-css');
var o_uglifyJS = require("uglify-js");
var n_callbacks = a_htmlFiles.length + a_cssFiles.length + a_jsFiles.length;

o_fileSystem.readFile(s_result, 'utf8', function(s_error, s_result) {
	if (s_error)
		return console.log(s_error);
	a_result = s_result.split("\n");

	a_htmlFiles.forEach(function(a_file) {
		console.log('processing ' + a_file.source);
		o_fileSystem.readFile(a_file.source, 'utf8', function(s_error, s_data) {
			if (s_error)
				return console.log(s_error);

			var s_compressed = o_minify(s_data, {
				caseSensitive: true,
				collapseBooleanAttributes: true,
				collapseWhitespace: true,
				quoteCharacter: "'",
				removeAttributeQuotes: true,
				removeComments: true
			});
			s_compressed = s_compressed.replace(/\"/g, '\\"');
			//console.log('size: ' + s_compressed.length());
			a_result = f_injectResult(s_compressed, a_file.regexp, a_result);
			n_callbacks--;
			if (!n_callbacks)
				f_saveResult(a_result);
		});
	});

	a_cssFiles.forEach(function(a_file) {
		console.log('processing ' + a_file.source);
		o_fileSystem.readFile(a_file.source, 'utf8', function(s_error, s_data) {
			if (s_error)
				return console.log(s_error);

			var a_compressed = new o_cleanCSS({}).minify(s_data);
			a_result = f_injectResult(a_compressed.styles, a_file.regexp, a_result)
			n_callbacks--;
			if (!n_callbacks)
				f_saveResult(a_result);
		});
	});

	a_jsFiles.forEach(function(a_file) {
		console.log('processing ' + a_file.source);
		o_fileSystem.readFile(a_file.source, 'utf8', function(s_error, s_data) {
			if (s_error)
				return console.log(s_error);

			var a_compressed = o_uglifyJS.minify(s_data);
			var s_compressed = a_compressed.code.replace(/\\/g, '\\\\').replace(/\"/g, '\\"');
			a_result = f_injectResult(s_compressed, a_file.regexp, a_result)
			n_callbacks--;
			if (!n_callbacks)
				f_saveResult(a_result);
		});
	});



});
