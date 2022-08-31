set xrange [0:0.033]
set yrange [-1:1]
plot "plot.dat" using 1:2 with lines
pause .5
reread
