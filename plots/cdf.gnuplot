# vim: textwidth=0

set ylabel "CDF"
set xlabel "latency (ms)"

set xtics 100
set mytics 2
set mxtics 2

set grid ytics back lt 0 lw 0.5
set grid xtics back lt 0 lw 0.5
set grid mxtics back lt 0 lw 0.5
set grid mytics back lt 0 lw 0.5

show grid

# Put the "key" outside the canvas
set key outside

set style fill pattern border -1
set xtics rotate by -30

set terminal svg fsize 14 dynamic size 900,500

set title 'Get latency CDF'
set output 'get-latency-cdf.svg'

plot 'get-latency-cdf.csv' index '[openchord]' using 1:2      \
     title 'openchord' with lines,                            \
     'get-latency-cdf.csv' index '[owshellchord]' using 1:2   \
     title 'owshellchord' with lines,                         \
     'get-latency-cdf.csv' index '[owshellkad]' using 1:2     \
     title 'owshellkad' with lines,                           \
     'get-latency-cdf.csv' index '[owshellpastry]' using 1:2  \
     title 'owshellpastry' with lines,                        \
     'get-latency-cdf.csv' index '[pastconsole]' using 1:2    \
     title 'pastconsole' with lines


set title 'Put latency CDF'
set output 'put-latency-cdf.svg'

plot 'put-latency-cdf.csv' index '[openchord]' using 1:2      \
     title 'openchord' with lines,                            \
     'put-latency-cdf.csv' index '[owshellchord]' using 1:2   \
     title 'owshellchord' with lines,                         \
     'put-latency-cdf.csv' index '[owshellkad]' using 1:2     \
     title 'owshellkad' with lines,                           \
     'put-latency-cdf.csv' index '[owshellpastry]' using 1:2  \
     title 'owshellpastry' with lines,                        \
     'put-latency-cdf.csv' index '[pastconsole]' using 1:2    \
     title 'pastconsole' with lines
