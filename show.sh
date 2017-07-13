#!/bin/sh
# Copyright © 2013 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.
SCRIPT=/tmp/show.$$
trap "rm $SCRIPT" 0 1 2 3 15
cat >$SCRIPT <<EOF
set term wxt persist;
set xlabel 'generation'
set ylabel 'time (sim secs)'
plot '$1' using 1:2 with lines title 'max';
replot '$1' using 1:3 with lines title 'avg';
EOF
gnuplot $SCRIPT
