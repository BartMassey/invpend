#!/bin/sh
# Copyright © 2013 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.
( time ./invpend -s -p $1 -g $2 |
  awk '{print $2, $4, $6;}' ) >${1}x${2}.stats 2>${1}x${2}.times
