# Inverted Pendulum Genetic Algorithm in C
Copyright &copyr; 2013 Bart Massey

This small program is a demo written largely in my Fall 2013
CS 441/541 Intro AI class at Portland State University. It
contains a (probably borked) model of the
[inverted pendulum](http://en.wikipedia.org/wiki/Inverted_pendulum)
problem, together with a GA-based optimizer that tries to
position the cart in a completely open-loop fashion to
maximize the time that the pendulum will stay balanced while
the cart remains on the track bounds.

The genome is just a list of changes to cart position over
time. New instances are produced by crossover taking an
arbitrary prefix of one parent to an arbitrary suffix of
another. Selection is by rank-ordering. Mutation is by
replacement of a single cart position in the genome.

There are a few arguments that can be discovered by reading
the code. The code also contains some hard-coded constants.
Some scripts are available for running the program and
plotting stats with [gnuplot](http://www.gnuplot.info/).

This program is licensed under the "MIT License". Please see
the file `COPYING` in this distribution for license terms.
