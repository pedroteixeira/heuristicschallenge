set terminal png size 600, 400
set output "2graph.png"

set title "valores com steiner-2"
set xlabel "iteracoes"
set ylabel "custo steiner tree"

plot "2graph.dat" using 1:2 with lines title "valor"