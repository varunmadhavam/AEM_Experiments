set xrange [0:50]
set yrange [0:700]
plot "dft.dat" using 1:2 with lines
pause .5
reread
