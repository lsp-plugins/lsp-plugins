# plot1d.gdb
#
# Copyright (C) 2008 Florian Lorenzen
#
# Plot an expression that expands to {x1, x2, ..., xN}, i. e.
# N numbers using gnuplot.
#
# This file is for the GNU debugger 6.x.
#
# It writes temporary files named __plot1d.dump, __plot1d.dat, __plot1d.gp, so
# you should not have files of the same name in the working directory.
#
# It requires sed, awk, and gnuplot available in the $PATH.

# plot1d_opt_range <expr> <opt> <range>
#
# Plot the points of <expr> passing <opt> as plot options using
# <range> in the set yrange command.
define plot1d_opt_range
  shell rm -f __plot1d.dump __plot1d.dat __plot1d.gp
  set logging file __plot1d.dump
  set logging on
  output $arg0
  set logging off
  shell awk '{printf("%s", $0)}' < __plot1d.dump | \
    sed 's/^{\(.*\)}$/\1/;s/, */\n/g' > __plot1d.dat
  shell echo "set yrange $arg2; plot '__plot1d.dat' $arg1 title '$arg0'; \
    pause -1 \"Press enter to continue\"" > __plot1d.gp
  shell gnuplot __plot1d.gp
  shell rm -f __plot1d.dump __plot1d.dat __plot1d.gp
end

# plot1d <expr>
#
# Just plot the points of <expr>.
define plot1d
  plot1d_opt_range $arg0 "" "[*:*]"
end

# plot1d_opt <expr> <opt>
#
# Plot the points of <expr> passing <opt> to the
# plot command after the datafile. So, one can pass
# "with lines" here.
define plot1d_opt
  plot1d_opt_range $arg0 $arg1 "[*:*]"
end

