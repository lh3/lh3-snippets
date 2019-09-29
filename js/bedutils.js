#!/usr/bin/env k8

var paftools_version = '2.15-r905';

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

function bed_sum(args)
{
	var buf = new Bytes();
	var file = args.length == 0 || args[0] == '-'? new File() : new File(args[0]);
	var s = 0;
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t", 3);
		if (t.length < 3) continue;
		s += parseInt(t[2])  - parseInt(t[1]);
	}
	file.close();
	buf.destroy();
	print(s);
	return 0;
}

function bed_sum2nd(args)
{
	var buf = new Bytes();
	var file = args.length == 0 || args[0] == '-'? new File() : new File(args[0]);
	var s = 0;
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t", 2);
		s += parseInt(t[1]);
	}
	file.close();
	buf.destroy();
	print(s);
	return 0;
}

function bed_merge(args)
{
	var buf = new Bytes();
	var file = args.length > 0? new File(args[0]) : new File();
	var ctg = null, st, en;
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t", 3);
		var s = parseInt(t[1]);
		var e = parseInt(t[2]);
		if (ctg != t[0] || s > en) { // no overlap
			if (ctg != null) print(ctg, st, en);
			ctg = t[0], st = s, en = e;
		} else if (s < st) throw Error("ERROR: input is not sorted by coordinate");
		else en = en > e? en : e;
	}
	if (ctg != null) print(ctg, st, en);
	file.close();
	buf.destroy();
	return 0;
}

function bed_sum1(args)
{
	var buf = new Bytes();
	var file = args.length == 0 || args[0] == '-'? new File() : new File(args[0]);
	var ctg = null, st = 0, en = 0, sum = 0;
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t", 3);
		var s = parseInt(t[1]);
		var e = parseInt(t[2]);
		if (ctg != t[0] || s > en) { // no overlap
			sum += en - st;
			if (ctg != null && ctg != t[0]) {
				print(ctg, sum);
				sum = 0;
			}
			ctg = t[0], st = s, en = e;
		} else if (s < st) throw Error("ERROR: input is not sorted by coordinate");
		else en = en > e? en : e;
	}
	if (ctg != null) {
		sum += en - st;
		print(ctg, sum);
	}
	file.close();
	buf.destroy();
	return 0;
}

function main(args)
{
	if (args.length == 0) {
		print("Usage: bedutils.js <command> [arguments]");
		print("Commands:");
		print("  sum        sum of BED regions");
		print("  sum1       sum of BED regions for each contig");
		print("  sum2nd     sum of the 2nd column");
		print("  merge      merge overlapping regions in sorted BED");
		exit(1);
	}

	var cmd = args.shift();
	if (cmd == 'sum') bed_sum(args);
	else if (cmd == 'sum2nd') bed_sum2nd(args);
	else if (cmd == 'sum1') bed_sum1(args);
	else if (cmd == 'merge') bed_merge(args);
	else throw Error("unrecognized command: " + cmd);
}

main(arguments);
