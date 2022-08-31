#compiling
gcc q1.c -lm -o q1
#running
./q1 -> will produce datapoints of a sinewave with frequency 100HZ, PHASE=0 and Amplitude=1.
./q1 200 45 .5 -> will produce datapoints of a sinewave with frequency 200HZ, PHASE=45 and Amplitude=.5.

#plotting
requires gnuplot
./run.sh -> will start the program by plotiing a 100HZ sinewave with 0 phase and 1 amplitude
use p or P to decrease/increase phase
use f or F to decrease/increase frequency
use Z to sweep from 30HZ to 300HZ -> enter any key in sweep to exit sweep
enter exit to exit the program
