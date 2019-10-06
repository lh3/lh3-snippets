#!/usr/bin/env k8

/*****************************
 ***** Library functions *****
 *****************************/

/*******************************
 * Command line option parsing *
 *******************************/

var getopt = function(args, ostr) {
	var oli; // option letter list index
	if (typeof(getopt.place) == 'undefined')
		getopt.ind = 0, getopt.arg = null, getopt.place = -1;
	if (getopt.place == -1) { // update scanning pointer
		if (getopt.ind >= args.length || args[getopt.ind].charAt(getopt.place = 0) != '-') {
			getopt.place = -1;
			return null;
		}
		if (getopt.place + 1 < args[getopt.ind].length && args[getopt.ind].charAt(++getopt.place) == '-') { // found "--"
			++getopt.ind;
			getopt.place = -1;
			return null;
		}
	}
	var optopt = args[getopt.ind].charAt(getopt.place++); // character checked for validity
	if (optopt == ':' || (oli = ostr.indexOf(optopt)) < 0) {
		if (optopt == '-') return null; //  if the user didn't specify '-' as an option, assume it means null.
		if (getopt.place < 0) ++getopt.ind;
		return '?';
	}
	if (oli+1 >= ostr.length || ostr.charAt(++oli) != ':') { // don't need argument
		getopt.arg = null;
		if (getopt.place < 0 || getopt.place >= args[getopt.ind].length) ++getopt.ind, getopt.place = -1;
	} else { // need an argument
		if (getopt.place >= 0 && getopt.place < args[getopt.ind].length)
			getopt.arg = args[getopt.ind].substr(getopt.place);
		else if (args.length <= ++getopt.ind) { // no arg
			getopt.place = -1;
			if (ostr.length > 0 && ostr.charAt(0) == ':') return ':';
			return '?';
		} else getopt.arg = args[getopt.ind]; // white space
		getopt.place = -1;
		++getopt.ind;
	}
	return optopt;
}

/******************************
 ***** Command-line tools *****
 ******************************/

function cos_cna2bed(args)
{
	var buf = new Bytes();
	var file = args.length == 0? new File() : new File(args[0]);
	var cnv = {};
	while (file.readline(buf) >= 0) {
		var m, t = buf.toString().split("\t");
		if (cnv[t[0]] != null) continue;
		cnv[t[0]] = true;
		if ((m = /^(\S+):(\d+)\.\.(\d+)/.exec(t[19])) == null) continue;
		var cn = /^\d+$/.test(t[14])? t[14] : '*';
		print(m[1], m[2], m[3], t[5], t[9], t[16], cn, t[13], t[0]);
	}
	file.close();
	buf.destroy();
}

function cos_cnasel(args)
{
	var max_dist = 100000, mode = 1, breakpoint = false;
	var c, file, buf = new Bytes();
	while ((c = getopt(args, "d:m:b")) != null) {
		if (c == 'd') max_dist = parseInt(getopt.arg);
		else if (c == 'b') breakpoint = true;
		else if (c == 'm') {
			if (getopt.arg == 'telo') mode = 1;
			else if (getopt.arg == 'arm') mode = 2;
			else throw("ERROR: unrecognized mode");
		}
	}
	if (args.length - getopt.ind == 0) {
		print("Usage: cosmic.js cnasel [options] <hs38.cen> <cna2bed.bed>");
		print("Options:");
		print("  -m STR     mode: telo, arm [telo]");
		print("  -d INT     max distance from telomeres (effective with -m telo) [" + max_dist + "]");
		print("  -b         output BED for CNA break points");
		print("Note: for human centromeres, see https://github.com/lh3/lh3-snippets/tree/master/data");
		exit(1);
	}

	var len = {};
	file = new File(args[getopt.ind]);
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t");
		if (t.length < 2) continue;
		len[t[0]] = [parseInt(t[3]), parseInt(t[1]), parseInt(t[2])];
	}
	file.close();

	file = args.length - getopt.ind < 2? new File() : new File(args[getopt.ind+1]);
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t");
		var st = parseInt(t[1]), en = parseInt(t[2]);
		if (len[t[0]] == null) continue;
		var l = len[t[0]][0], cs = len[t[0]][1], ce = len[t[0]][2];
		if (mode == 1) {
			if (st > max_dist && l - en > max_dist) continue;
			if (st < max_dist && l - en < max_dist) continue;
			var dir = "";
			if (st < max_dist) {
				if (en > cs) continue;
				dir = "short";
			} else {
				if (st < ce) continue;
				dir = "long";
			}
			if (breakpoint) {
				if (dir == "short") t[1] = t[2] - 1;
				else t[2] = t[1] + 1;
				print(t.join("\t"), dir);
			} else print(buf, dir);
		} else if (mode == 2) {
			var dir = null;
			if (st > ce) dir = "long";
			else if (en < cs) dir = "short";
			if (dir) {
				if (breakpoint) {
					var st = t[1], en = t[2];
					t[1] = st, t[2] = st + 1;
					print(t.join("\t"), dir);
					t[1] = en - 1, t[2] = en;
					print(t.join("\t"), dir);
				} else print(buf, dir);
			}
		} else throw("ERROR: not implemented");
	}
	file.close();

	buf.destroy();
}

/*************************
 ***** Main function *****
 *************************/

function main(args)
{
	if (args.length == 0) {
		print("Usage: cosmic.js <command> [arguments]");
		print("Commands:");
		print("  cna2bed     convert CompleteCNA to BED");
		print("  cnasel      select CNAs");
		exit(1);
	}

	var cmd = args.shift();
	if (cmd == 'cna2bed') cos_cna2bed(args);
	else if (cmd == 'cnasel') cos_cnasel(args);
	else throw Error("unrecognized command: " + cmd);
}

main(arguments);
