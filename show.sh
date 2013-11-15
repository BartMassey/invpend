#!/bin/sh
SCRIPT=/tmp/show.$$
trap "rm $SCRIPT" 0 1 2 3 15
cat >$SCRIPT <<EOF
set term wxt persist;
plot '$1' using 1:2 with lines;
replot '$1' using 1:3 with lines;
EOF
gnuplot $SCRIPT
