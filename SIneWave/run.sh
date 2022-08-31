F=100
P=0
A=1
gnuplot liveplot.gnu &
PID=$!
while true; do
./q1 $F $P $A > plot.dat
read input
if [ "$input" == "exit" ]
    then
        break
else
    case $input in
        A)
            if [ $A -ne 1 ] 
            then
                ((A=A+.01))
                echo A=$A;
            fi
            ;;
        a)
            if [ $A -ne .01 ] 
            then
                ((A=A-.01))
                echo A=$A;
            fi
            ;;
        F)
            if [ $F -ne 290 ] 
            then
                ((F=F+1))
                echo F=$F;
            fi
            ;;
        f)
            if [ $F -ne 40 ] 
            then
                ((F=F-1))
            fi
            echo F=$F;
            ;;
        P)
            if [ $P -ne 360 ] 
            then
                ((P=P+1))
                echo P=$P;
            fi
            ;;
        p)
            if [ $P -ne 0 ] 
            then
                ((P=P-1))
                echo P=$P;
            fi
            ;;
        Z)
            state=0
            ((F=100))
            while true; do
            if [ $state -eq 0 ]
            then
                if [ $F -eq 300 ]
                then
                    ((state=1))
                    continue
                else
                    ((F=F+10))
                fi
            else
                if [ $F -eq 30 ]
                then
                    ((state=0))
                    continue
                else
                    ((F=F-10))
                fi
            fi
            echo F=$F;
            read -t 1 in
            if [ $? -eq 0 ]
            then
                break
            fi
            ./q1 $F $P $A > plot.dat
            done
    esac
fi
done
kill -9 $PID
killall -q gnuplot_qt
