#!/usr/bin/env k8

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

var c, height = 1.5, y_max0 = null, y_min0 = null;
while ((c = getopt(arguments, "h:x:i:")) != null) {
	if (c == 'h') height = parseFloat(getopt.arg);
	else if (c == 'x') y_max0 = parseFloat(getopt.arg);
	else if (c == 'i') y_min0 = parseFloat(getopt.arg);
}

if (arguments.length - getopt.ind < 2) {
	print("Usage: chr-plot.js [options] <chr.size> <dat.txt>");
	exit(1);
}

var file, buf = new Bytes();

var chr_list = [], chr = {}, cen = [], max_len = 0;
file = new File(arguments[getopt.ind]);
while (file.readline(buf) >= 0) {
	var t = buf.toString().split("\t");
	var len = parseInt(t[3]);
	chr_list.push(t[0]);
	cen.push([parseInt(t[1]), parseInt(t[2])]);
	chr[t[0]] = len;
	max_len = max_len > len? max_len : len;
}
file.close();

var y_max = -1e300, y_min = 1e300;
file = new File(arguments[getopt.ind+1]);
while (file.readline(buf) >= 0) {
	var t = buf.toString().split("\t");
	if (chr[t[0]] == null) continue;
	var y = parseFloat(t[2]);
	y_max = y_max > y? y_max : y;
	y_min = y_min < y? y_min : y;
}
file.close();
if (y_max0 != null) y_max = y_max0;
if (y_min0 != null) y_min = y_min0;

buf.destroy();

print('set t po eps co so enh "Helvetica,14"');
print('set out "chr-plot.eps"');
print('set size 1,' + (height + 0.02));
print('set multiplot layout ' + chr_list.length + ',1');
print('set lmargin screen 0.07');
print('set border 0; unset xtics; unset ytics; set bmargin 0; set tmargin 0.02; set rmargin 0');
print('set style line 1 lc rgb "#e41a1c" lw 1');
print('set style fill transparent solid 0.5 noborder');
print('set yran [' + y_min + ':' + y_max + ']');
print('');
var h = height / chr_list.length;
for (var i = 0; i < chr_list.length; ++i) {
	var len = chr[chr_list[i]];
	print('set origin 0,' + (height - (i + 1) * h + 0.01));
	print('set xran [0:' + len * 1e-6 + ']');
	print('set size ' + (len/max_len) + ',' + h);
	print('set style rect fc lt -1 fs solid 0.15 noborder');
	print('unset obj; unset label');
	print('set obj rect from ' + cen[i][0]*1e-6 + ', graph 0 to ' + cen[i][1]*1e-6 + ', graph 1');
	print('set label "' + chr_list[i] + '" at screen 0.01, graph 0.5');
	print('plot "<awk \'$1==\\"' + chr_list[i] + '\\"\' ' + arguments[getopt.ind+1] + '" u ($2*1e-6):(0):($3) not w filledcu ls 1');
}
