#compiling
gcc dft.c -lm -o dft
#plotting
requires gnuplot
./run.sh -> will start the program by plotiing a 100HZ sinewave with 0 phase and 1 amplitude
use p or P to decrease/increase phase
use f or F to decrease/increase frequency
use Z to sweep from 30HZ to 300HZ -> enter any key in sweep to exit sweep
enter exit to exit the program
