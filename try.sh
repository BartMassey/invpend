#!/bin/sh
( time ./invpend -s -p $1 -g $2 |
  awk '{print $2, $4, $6;}' ) >${1}x${2}.stats 2>${1}x${2}.times
