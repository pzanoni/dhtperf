# vim: textwidth=0

set ylabel "latency (us)"
#set ylabel "latência (us)"

set yrange [0:1000000.0]

#set ytics 0.2
set mytics 2

set grid ytics back lt 0 lw 0.5
set grid mytics back lt 0 lw 0.5
show grid

set boxwidth 1 relative
set style data histograms
#set style fill solid 1.0 border -1
set style fill pattern border -1
#set palette gray
#set datafile separator ","
set xtics rotate by -30

#set terminal png truecolor font arial 8 size 800,500
#set output 'latency.png'

set terminal svg fsize 16


set title "Get latency"
set output 'get-latency.svg'

#set terminal postscript eps monochrome font 11
#set output 'get-latency.eps'

plot 'get-latency.csv' using 2:xticlabels(1) title 'Min',           \
     'get-latency.csv' using 3:xticlabels(1) title 'Min mean',      \
     'get-latency.csv' using 4:xticlabels(1) title 'Mean',          \
     'get-latency.csv' using 5:xticlabels(1) title 'Mean of means', \
     'get-latency.csv' using 6:xticlabels(1) title 'Max mean',      \
     'get-latency.csv' using 7:xticlabels(1) title 'Max'

set title "Put latency"
set output 'put-latency.svg'

#set terminal postscript eps monochrome font 11
#set output 'put-latency.eps'

plot 'put-latency.csv' using 2:xticlabels(1) title 'Min',           \
     'put-latency.csv' using 3:xticlabels(1) title 'Min mean',      \
     'put-latency.csv' using 4:xticlabels(1) title 'Mean',          \
     'put-latency.csv' using 5:xticlabels(1) title 'Mean of means', \
     'put-latency.csv' using 6:xticlabels(1) title 'Max mean',      \
     'put-latency.csv' using 7:xticlabels(1) title 'Max'
